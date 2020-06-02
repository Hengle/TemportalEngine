#include "Editor.hpp"

#include "Engine.hpp"
#include "Window.hpp"
#include "graphics/ImGuiRenderer.hpp"

#include <memory>
#include "memory/MemoryChunk.hpp"
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_vulkan.h>

Editor* Editor::EDITOR = nullptr;

Editor::Editor(std::shared_ptr<memory::MemoryChunk> mainMemory, std::unordered_map<std::string, ui64> memoryChunkSizes)
{
	assert(EDITOR == nullptr);
	EDITOR = this;

	auto pEngine = engine::Engine::Create(mainMemory, memoryChunkSizes);
	pEngine->setProject(mainMemory->make_shared<asset::Project>(asset::Project("Editor", TE_GET_VERSION(TE_MAKE_VERSION(0, 0, 1)))));

	this->mDockspace = gui::MainDockspace("Editor::MainDockspace", "Editor");
	this->mDockspace.open();
}

Editor::~Editor()
{
	EDITOR = nullptr;
	engine::Engine::Destroy();
}

bool Editor::setup(utility::ArgumentMap args)
{
	this->mbShouldRender = args.find("noui") == args.end();
	auto pEngine = engine::Engine::Get();
	if (!pEngine->initializeDependencies(this->mbShouldRender)) return false;
	if (this->mbShouldRender && !pEngine->setupVulkan()) return false;
	return true;
}

void Editor::run()
{
	if (!this->mbShouldRender)
	{
		// Running a headless command
		DeclareLog("Editor").log(logging::ECategory::LOGINFO, "no u");
		return;
	}

	this->mpEngine = engine::Engine::Get();

	this->mpWindow = this->mpEngine->createWindow(800, 600, "Editor", WindowFlags::RESIZABLE);
	if (this->mpWindow == nullptr) return;
	
	auto pVulkan = this->mpEngine->initializeVulkan(this->mpWindow->querySDLVulkanExtensions());
	auto renderer = graphics::ImGuiRenderer(pVulkan, this->mpWindow->createSurface().initialize(pVulkan));
	this->initializeRenderer(&renderer);
	renderer.finalizeInitialization();

	this->mpWindow->setRenderer(&renderer);

	renderer.addGui(&this->mDockspace);

	this->mpEngine->start();
	while (this->mpEngine->isActive() && !this->mpWindow->isPendingClose() && this->mDockspace.isOpen())
	{
		this->mpEngine->update();
	}
	this->mpEngine->joinThreads();

	renderer.removeGui(&this->mDockspace);

	renderer.invalidate();
	this->mpEngine->destroyWindow(this->mpWindow);
}

void Editor::initializeRenderer(graphics::VulkanRenderer *pRenderer)
{
	pRenderer->setPhysicalDevicePreference(
		graphics::PhysicalDevicePreference()
		.addCriteriaQueueFamily(graphics::QueueFamily::eGraphics)
		.addCriteriaQueueFamily(graphics::QueueFamily::ePresentation)
	);
	pRenderer->setLogicalDeviceInfo(
		graphics::LogicalDeviceInfo()
		.addQueueFamily(graphics::QueueFamily::eGraphics)
		.addQueueFamily(graphics::QueueFamily::ePresentation)
		.addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
		.setValidationLayers(engine::Engine::VulkanValidationLayers)
	);
	pRenderer->setSwapChainInfo(
		graphics::SwapChainInfo()
		.addFormatPreference(vk::Format::eB8G8R8A8Unorm)
		.addFormatPreference(vk::Format::eR8G8B8A8Unorm)
		.addFormatPreference(vk::Format::eB8G8R8Unorm)
		.addFormatPreference(vk::Format::eR8G8B8Unorm)
		.setColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
#ifdef IMGUI_UNLIMITED_FRAME_RATE
		.addPresentModePreference(vk::PresentModeKHR::eMailbox)
		.addPresentModePreference(vk::PresentModeKHR::eImmediate)
#endif
		.addPresentModePreference(vk::PresentModeKHR::eFifo)
	);
	pRenderer->setImageViewInfo({
		vk::ImageViewType::e2D,
		{
			vk::ComponentSwizzle::eR,
			vk::ComponentSwizzle::eG,
			vk::ComponentSwizzle::eB,
			vk::ComponentSwizzle::eA
		}
	});

	pRenderer->initializeDevices();
	pRenderer->createRenderChain();
}

bool Editor::hasProject() const
{
	return (bool)this->mpProject;
}

void Editor::setProject(asset::AssetPtrStrong asset)
{
	asset::ProjectPtrStrong project = std::dynamic_pointer_cast<asset::Project>(asset);
	assert(project != nullptr);
	this->mpProject = project;

	if (this->mpWindow)
	{
		this->mpWindow->setTitle("Editor: " + this->getProject()->getDisplayName());
	}
	
	asset::AssetManager::get()->scanAssetDirectory(this->mpProject->getAssetDirectory());
}

asset::ProjectPtrStrong Editor::getProject()
{
	return this->mpProject;
}
