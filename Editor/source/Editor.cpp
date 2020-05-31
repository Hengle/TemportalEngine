#include "Editor.hpp"

#include "Engine.hpp"
#include "Window.hpp"
#include "graphics/ImGuiRenderer.hpp"

#include <memory>
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_vulkan.h>

Editor* Editor::EDITOR = nullptr;

Editor::Editor()
{
	assert(EDITOR == nullptr);
	EDITOR = this;

	std::string logFileName = "TemportalEngine_Editor_" + logging::LogSystem::getCurrentTimeString() + ".log";
	engine::Engine::LOG_SYSTEM.open(logFileName.c_str());

	auto pEngine = engine::Engine::Create();

	utility::SExecutableInfo appInfo = { "Editor", TE_MAKE_VERSION(0, 0, 1) };
	pEngine->setApplicationInfo(&appInfo);

	this->mDockspace = gui::MainDockspace("Editor::MainDockspace", "Editor");
	this->mDockspace.open();
}

Editor::~Editor()
{
	EDITOR = nullptr;

	engine::Engine::Destroy();
	engine::Engine::LOG_SYSTEM.close();
}

bool Editor::setup()
{
	auto pEngine = engine::Engine::Get();
	if (!pEngine->initializeDependencies()) return false;
	if (!pEngine->setupVulkan()) return false;
	return true;
}

void Editor::run()
{
	// TODO: moved the shared_ptr wrapper to Engine::Get()
	this->mpEngine = engine::Engine::Get();

	auto pWindow = this->mpEngine->createWindow(800, 600, WindowFlags::RESIZABLE);
	if (pWindow == nullptr) return;
	
	auto pVulkan = this->mpEngine->initializeVulkan(pWindow->querySDLVulkanExtensions());
	auto renderer = graphics::ImGuiRenderer(pVulkan, pWindow->createSurface().initialize(pVulkan));
	this->initializeRenderer(&renderer);
	renderer.finalizeInitialization();

	pWindow->setRenderer(&renderer);

	renderer.addGui(&this->mDockspace);

	this->mpEngine->start();
	while (this->mpEngine->isActive() && !pWindow->isPendingClose() && this->mDockspace.isOpen())
	{
		this->mpEngine->update();
	}
	this->mpEngine->joinThreads();

	renderer.removeGui(&this->mDockspace);

	renderer.invalidate();
	this->mpEngine->destroyWindow(pWindow);
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
}

asset::ProjectPtrStrong Editor::getProject()
{
	return this->mpProject;
}
