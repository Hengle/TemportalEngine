#include "Engine.hpp"

#include "ITickable.hpp"
#include "Window.hpp"
#include "ecs/component/Transform.hpp"
#include "graphics/VulkanRenderer.hpp"
#include "input/Queue.hpp"
#include "memory/MemoryChunk.hpp"

//#include "network/client/ServiceClient.hpp"
//#include "network/server/ServiceServer.hpp"

#include <string>

using namespace engine;

#define ENGINE_VERSION TE_MAKE_VERSION(1, 0, 0)
#define GET_MEMORY_SIZE(sizes, key, defaultSize) uSize(sizes.find(key) != sizes.end() ? sizes.find(key)->second : defaultSize)

logging::LogSystem Engine::LOG_SYSTEM = logging::LogSystem();
Engine::EnginePtr Engine::spInstance = nullptr;
std::shared_ptr<memory::MemoryChunk> Engine::spMainMemory = nullptr;

std::vector<std::string> Engine::VulkanValidationLayers = { "VK_LAYER_KHRONOS_validation" };

void Engine::startLogSystem(std::string const name)
{
	std::string logFileName = name + "_" + logging::LogSystem::getCurrentTimeString() + ".log";
	engine::Engine::LOG_SYSTEM.open(logFileName.c_str());
}

void Engine::stopLogSystem()
{
	engine::Engine::LOG_SYSTEM.close();
}

#pragma region Singleton

Engine::EnginePtr Engine::Create(std::unordered_map<std::string, uSize> memoryChunkSizes)
{
	assert(Engine::spInstance == nullptr);
	spMainMemory = memory::MemoryChunk::Create(GET_MEMORY_SIZE(memoryChunkSizes, "main", 1 << 16));
	Engine::spInstance = spMainMemory->make_shared<Engine>(spMainMemory, memoryChunkSizes);
	Engine::spInstance->initializeInput();
	return Engine::Get();
}

Engine::EnginePtr Engine::Get()
{
	return Engine::spInstance;
}

void Engine::Destroy()
{
	if (!Engine::spInstance) return;

	assert(Engine::spInstance.use_count() == 1);
	Engine::spInstance = nullptr;
	assert(!Engine::spInstance);

	assert(Engine::spMainMemory.use_count() == 1);
	Engine::spMainMemory = nullptr;
	assert(!Engine::spMainMemory);
}

#pragma endregion

Engine::Engine(std::shared_ptr<memory::MemoryChunk> mainMemory, std::unordered_map<std::string, uSize> memoryChunkSizes)
	: mpMainMemory(mainMemory)
	, mbShouldContinueRunning(false)
	//, mpNetworkService(nullptr)
{
	LogEngineInfo("Creating Engine");
	mEngineInfo.title = "TemportalEngine";
	mEngineInfo.version = ENGINE_VERSION;

	this->mMiscMemory = memory::MemoryChunk::Create(GET_MEMORY_SIZE(memoryChunkSizes, "misc", 1 << 16));

	this->mpInputWatcher[0] = input::InputWatcher(std::bind(
		&Engine::onRawEvent, this, std::placeholders::_1
	));
	this->mpInputWatcher->setInputEventCallback(std::bind(
		&Engine::enqueueInput, this, std::placeholders::_1
	));

	this->mpAssetManager = this->mpMainMemory->make_shared<asset::AssetManager>();
	this->mpAssetManager->setAssetMemory(memory::MemoryChunk::Create(GET_MEMORY_SIZE(memoryChunkSizes, "asset", 1 << 16)));
}

Engine::~Engine()
{
	// TODO: deallocate any windows that still exist in here
	this->mWindowPtrs.clear();

	if (this->mVulkanInstance.isValid())
	{
		this->mVulkanInstance.destroy();
	}
	this->terminateDependencies();

	// A good deconstructor example of how to unbind, but not necessary for the engine to do
	this->mpInputQueue->OnInputEvent.unbindExpired(input::EInputType::QUIT);

	this->mpInputQueue.reset();
	assert(!this->mpInputQueue);
	
	this->mProject.reset();
	assert(!this->mProject);
	this->mpAssetManager.reset();
	assert(!this->mpAssetManager);

	assert(this->mMiscMemory.use_count() == 1);
	this->mMiscMemory.reset();
	assert(!this->mMiscMemory);

	this->mpMainMemory.reset();
	assert(!this->mpMainMemory);

	LogEngineInfo("Engine Destroyed");
}

void Engine::initializeInput()
{
	// TODO: allocate from a sub-chunk of main memory
	this->mpInputQueue = this->mpMainMemory->make_shared<input::Queue>();
	this->mpInputQueue->OnInputEvent.bind(
		input::EInputType::QUIT, this->weak_from_this(),
		std::bind(&Engine::processInput, this, std::placeholders::_1)
	);
}

std::shared_ptr<input::Queue> Engine::getInputQueue() const
{
	return this->mpInputQueue;
}

std::shared_ptr<asset::AssetManager> Engine::getAssetManager()
{
	return mpAssetManager;
}

void Engine::initializeECS()
{
	this->mECS.setLog(DeclareLog("ECS"));
	this->mECS.registerType<ecs::ComponentTransform>("Transform");
	this->mECS.constructComponentPools();
}

ecs::Core& Engine::getECS()
{
	return this->mECS;
}

bool Engine::hasProject() const
{
	return (bool)this->mProject;
}

void Engine::setProject(asset::ProjectPtrStrong project)
{
	this->mProject = project;
}

asset::ProjectPtrStrong Engine::getProject() const
{
	return this->mProject;
}

utility::SExecutableInfo const *const Engine::getInfo() const
{
	return &mEngineInfo;
}

#pragma region Memory

std::shared_ptr<memory::MemoryChunk> Engine::getMainMemory() const
{
	return this->mpMainMemory;
}

std::shared_ptr<memory::MemoryChunk> Engine::getMiscMemory() const
{
	return this->mMiscMemory;
}

#pragma endregion

#pragma region Dependencies

bool Engine::initializeDependencies(bool bShouldRender)
{
	if (bShouldRender)
	{
		if (!mpDepSDL->initialize()) return false;
	}
	return true;
}

void Engine::terminateDependencies()
{
	if (mpDepSDL->isInitialized())
		mpDepSDL->terminate();
}

#pragma endregion

#pragma region Windows

std::shared_ptr<Window> Engine::createWindow(ui16 width, ui16 height, std::string title, WindowFlags flags)
{
	// TODO: allocate from a sub-chunk of main memory
	auto window = this->mpMainMemory->make_shared<Window>(width, height, title, flags);
	window->addInputListeners(this->mpInputQueue);
	this->mWindowPtrs.insert(std::make_pair(window->getId(), window));
	return window;
}

void Engine::destroyWindow(std::shared_ptr<Window> &pWindow)
{
	this->mWindowPtrs.erase(pWindow->getId());
	pWindow->destroy();
}

#pragma endregion

#pragma region Graphics

bool Engine::setupVulkan()
{
	this->mVulkanInstance.setApplicationInfo(this->mProject->getName(), this->mProject->getVersion());
	this->mVulkanInstance.setEngineInfo(mEngineInfo);
	this->mVulkanInstance.setValidationLayers(Engine::VulkanValidationLayers);
	this->mVulkanInstance.createLogger(&LOG_SYSTEM, /*vulkan debug*/ true);
	return true;
}

graphics::VulkanInstance* Engine::initializeVulkan(std::vector<const char*> requiredExtensions)
{
	this->mVulkanInstance.setRequiredExtensions(requiredExtensions);
	this->mVulkanInstance.initialize();
	return &this->mVulkanInstance;
}

void Engine::initializeRenderer(graphics::VulkanRenderer *renderer, std::shared_ptr<Window> pWindow)
{
	renderer->setInstance(&this->mVulkanInstance);
	renderer->takeOwnershipOfSurface(pWindow->createSurface().initialize(&this->mVulkanInstance));
	renderer->setPhysicalDevicePreference(this->getProject()->getPhysicalDevicePreference());
	renderer->setLogicalDeviceInfo(this->getProject()->getGraphicsDeviceInitInfo());
#ifndef NDEBUG
	renderer->setValidationLayers(engine::Engine::VulkanValidationLayers);
#endif
	renderer->initializeDevices();
}

#pragma endregion

#pragma region Game Loop

void Engine::start()
{
	this->mbShouldContinueRunning = true;

	for (auto [id, pWindow] : this->mWindowPtrs)
	{
		pWindow->startThread();
	}

	if (this->hasNetwork())
	{
		//this->mpNetworkService->startThread(this);
	}
}

bool const Engine::isActive() const
{
	return this->mbShouldContinueRunning;
}

void Engine::update(f32 deltaTime)
{
	this->pollInput();
	mpInputQueue->dispatchAll();
	for (auto tickable : this->mTickers)
	{
		if (tickable.expired()) continue;
		tickable.lock()->tick(deltaTime);
	}
	for (auto[id, pWindow] : this->mWindowPtrs)
	{
		pWindow->update();
	}
}

void Engine::joinThreads()
{
	//if (this->hasNetwork())
	//	this->mpNetworkService->joinThread();

	for (auto[id, pWindow] : this->mWindowPtrs)
	{
		pWindow->joinThread();
	}
}

void Engine::markShouldStop()
{
	this->mbShouldContinueRunning = false;
}

#pragma endregion

#pragma region Input

void Engine::pollInput()
{
	mpInputWatcher->pollInput();
}

void Engine::enqueueInput(input::Event const & evt)
{
	this->mpInputQueue->enqueue(evt);
}

void Engine::processInput(input::Event const & evt)
{
	if (evt.type == input::EInputType::QUIT)
	{
		this->markShouldStop();
	}

	if (evt.type == input::EInputType::KEY
		&& evt.inputKey.action == input::EAction::PRESS
		&& evt.inputKey.key == input::EKey::ESCAPE)
	{
		this->markShouldStop();
	}

}

void Engine::onRawEvent(void *pEvt)
{
	for (auto [id, pWindow] : this->mWindowPtrs)
	{
		pWindow->onEvent(pEvt);
	}
}

#pragma endregion

void Engine::createClient(char const *address, ui16 port)
{
	LogEngine(logging::ECategory::LOGINFO, "Initializing network client");
	//auto client = this->alloc<network::ServiceClient>();
	//client->initialize();
	//client->connectToServer(address, port);
	//this->mpNetworkService = client;
}

void Engine::createServer(ui16 const port, ui16 maxClients)
{
	LogEngine(logging::ECategory::LOGINFO, "Initializing network server");
	//auto server = this->alloc<network::ServiceServer>();
	//server->initialize(port, maxClients);
	//this->mpNetworkService = server;
}

bool const Engine::hasNetwork() const
{
	return false;// this->mpNetworkService != nullptr;
}

/*
std::optional<network::Service* const> Engine::getNetworkService() const
{
	if (this->hasNetwork())
		return this->mpNetworkService;
	return std::nullopt;
}
//*/

void Engine::addTicker(std::weak_ptr<ITickable> tickable)
{
	this->mTickers.push_back(tickable);
}
