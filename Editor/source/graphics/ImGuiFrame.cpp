#include "graphics/ImGuiFrame.hpp"

#include "graphics/LogicalDevice.hpp"

using namespace graphics;

ImGuiFrame::ImGuiFrame()
{
}

ImGuiFrame::ImGuiFrame(ImGuiFrame &&other)
{
	*this = std::move(other);
}

ImGuiFrame::~ImGuiFrame()
{
	this->destroy();
}

ImGuiFrame& ImGuiFrame::operator=(ImGuiFrame&& other)
{
	mView.swap(other.mView);
	mQueueFamilyGroup = other.mQueueFamilyGroup;
	mFrameBuffer = std::move(other.mFrameBuffer);
	mCommandPool.swap(other.mCommandPool);
	mCommandBuffer.swap(other.mCommandBuffer);
	mFence_FrameInFlight.swap(other.mFence_FrameInFlight);
	mSemaphore_ImageAcquired.swap(other.mSemaphore_ImageAcquired);
	mSemaphore_RenderComplete.swap(other.mSemaphore_RenderComplete);
	other.destroy();
	return *this;
}

ImGuiFrame& ImGuiFrame::setRenderPass(RenderPass const *pRenderPass)
{
	mFrameBuffer.setRenderPass(pRenderPass);
	return *this;
}

ImGuiFrame& ImGuiFrame::setView(vk::UniqueImageView &view)
{
	mView.swap(view);
	return *this;
}

ImGuiFrame& ImGuiFrame::setQueueFamilyGroup(QueueFamilyGroup const &group)
{
	mQueueFamilyGroup = group;
	return *this;
}

ImGuiFrame& ImGuiFrame::create(LogicalDevice const *pDevice)
{
	// Create the frame buffer
	mFrameBuffer.setView(mView.get()).create(pDevice);

	// Create a command pool per frame (because IMGUI needs to record commands per frame)
	mCommandPool = pDevice->mDevice->createCommandPoolUnique(vk::CommandPoolCreateInfo()
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
		.setQueueFamilyIndex(mQueueFamilyGroup.getQueueIndex(graphics::QueueFamily::eGraphics).value())
	);
	
	// Create the command buffer for each frame
	auto cmdBuffers = pDevice->mDevice->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo()
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandPool(mCommandPool.get())
		.setCommandBufferCount(1)
	);
	mCommandBuffer.swap(cmdBuffers[0]);

	// Create fence to indicate that the frame is currently in-flight (and CPU can wait on it)
	// start the fence in the signaled state (aligns with its state when the frame is finished)
	mFence_FrameInFlight = pDevice->mDevice->createFenceUnique(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));

	// Create semaphores for GPU queue communication
	mSemaphore_ImageAcquired = pDevice->mDevice->createSemaphoreUnique(vk::SemaphoreCreateInfo());
	mSemaphore_RenderComplete = pDevice->mDevice->createSemaphoreUnique(vk::SemaphoreCreateInfo());

	return *this;
}

void ImGuiFrame::destroy()
{
	this->mSemaphore_RenderComplete.reset();
	this->mSemaphore_ImageAcquired.reset();
	this->mFence_FrameInFlight.reset();
	this->mCommandBuffer.reset();
	this->mCommandPool.reset();
	this->mFrameBuffer.destroy();
	this->mView.reset();
}
