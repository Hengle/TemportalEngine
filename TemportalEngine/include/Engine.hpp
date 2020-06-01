#ifndef TE_ENGINE_HPP
#define TE_ENGINE_HPP

#include "TemportalEnginePCH.hpp"

#include "WindowFlags.hpp"
#include "dependency/SDL.hpp"
#include "graphics/VulkanInstance.hpp"
#include "input/InputWatcher.hpp"
#include "input/types.h"
//#include "network/common/Service.hpp"
#include "thread/Thread.hpp"
#include "version.h"

#include "asset/Project.hpp"

#include "logging/Logger.hpp"
#include "asset/AssetManager.hpp"
#include "ExecutableInfo.hpp"

#include <optional>
#include <unordered_map>
#include <typeinfo>

class Window;

NS_MEMORY
class MemoryChunk;
NS_END

NS_INPUT
class Queue;
NS_END

NS_ENGINE

#define DeclareLog(title) logging::Logger(title, &engine::Engine::LOG_SYSTEM)
#define LogEngine(cate, ...) DeclareLog("Engine").log(cate, __VA_ARGS__);
#define LogEngineInfo(...) LogEngine(logging::ECategory::LOGINFO, __VA_ARGS__)
#define LogEngineDebug(...) LogEngine(logging::ECategory::LOGDEBUG, __VA_ARGS__)

class TEMPORTALENGINE_API Engine
{
private:
	static std::shared_ptr<Engine> spInstance;

public:
	typedef std::shared_ptr<Engine> EnginePtr;
	static logging::LogSystem LOG_SYSTEM;
	static std::vector<const char*> VulkanValidationLayers;

#pragma region Singleton
	static EnginePtr Create(std::shared_ptr<memory::MemoryChunk> pChunk, std::unordered_map<std::string, ui64> memoryChunkSizes);
	static EnginePtr Get();
	static void Destroy();
#pragma endregion

	Engine(std::shared_ptr<memory::MemoryChunk> mainMemory, std::unordered_map<std::string, ui64> memoryChunkSizes);
	~Engine();

	bool hasProject() const;
	void setProject(asset::ProjectPtrStrong project);
	asset::ProjectPtrStrong getProject() const;
	utility::SExecutableInfo const *const getInfo() const;
	
#pragma region Dependencies
	bool initializeDependencies();
	void terminateDependencies();
#pragma endregion

	std::shared_ptr<asset::AssetManager> getAssetManager() { return mpAssetManager; }

#pragma region Windows
	std::shared_ptr<Window> createWindow(ui16 width, ui16 height, std::string title, WindowFlags flags = WindowFlags::RENDER_ON_THREAD);
	void destroyWindow(std::shared_ptr<Window> &pWindow);
#pragma endregion

#pragma region Graphics
	bool setupVulkan();
	graphics::VulkanInstance* initializeVulkan(std::vector<const char*> requiredExtensions);
#pragma endregion

#pragma region Game Loop
	void start();
	bool const isActive() const;
	void update();
	void joinThreads();
	void markShouldStop();
#pragma endregion

	void createServer(ui16 const port, ui16 maxClients);
	void createClient(char const *address, ui16 port);
	bool const hasNetwork() const;
	//std::optional<network::Service* const> getNetworkService() const;

private:
	
	asset::ProjectPtrStrong mProject;

	utility::SExecutableInfo mEngineInfo;

#pragma region Memory
	std::shared_ptr<memory::MemoryChunk> mMainMemory;
#pragma endregion
	
#pragma region Dependencies
	dependency::SDL mpDepSDL[1];
#pragma endregion

	std::shared_ptr<asset::AssetManager> mpAssetManager;
	
#pragma region Windows
	std::map<ui32, std::shared_ptr<Window>> mWindowPtrs;
#pragma endregion
	
#pragma region Graphic
	graphics::VulkanInstance mVulkanInstance;
#pragma endregion

#pragma region Input
	input::InputWatcher mpInputWatcher[1];
	std::shared_ptr<input::Queue> mpInputQueue;
	input::ListenerHandle mInputHandle;
#pragma endregion

	bool mbShouldContinueRunning;

	//network::Service *mpNetworkService;

#pragma region Input
	void pollInput();
	void enqueueInput(struct input::Event const &evt);
	void onRawEvent(void *pEvt);
	void processInput(struct input::Event const &evt);
#pragma endregion

};

NS_END

#endif