#include "graphics/SwapChain.hpp"

#include "graphics/LogicalDevice.hpp"
#include "graphics/Surface.hpp"
#include "types/math.h"

using namespace graphics;

SwapChain::SwapChain()
{
}

SwapChain& SwapChain::setInfo(SwapChainInfo const &info)
{
	this->mInfo = info;
	return *this;
}

SwapChain& SwapChain::setSupport(SwapChainSupport const &support)
{
	this->mSupport = support;
	return *this;
}

SwapChain& SwapChain::setQueueFamilyGroup(QueueFamilyGroup const &qfg)
{
	this->mQueueFamilyGroup = qfg;
	return *this;
}

void SwapChain::create(LogicalDevice const *pDevice, Surface const *pSurface)
{
	mpDevice = pDevice;

	mDrawableSize = pSurface->getDrawableSize();
	mResolution = mInfo.getAdjustedResolution(mDrawableSize, mSupport.capabilities);
	mSurfaceFormat = mInfo.selectSurfaceFormat(mSupport.surfaceFormats);
	mPresentationMode = mInfo.selectPresentationMode(mSupport.presentationModes);
	// Adding at least 1 more to the chain will help avoid waiting on the GPU to complete internal ops before showing the next buffer.
	// Ensure that the image count does not exceed the max, unless the max is == 0
	ui32 minImageCount = minUnless(mSupport.capabilities.minImageCount + 1, mSupport.capabilities.maxImageCount, 0);

	auto info = vk::SwapchainCreateInfoKHR()
		// Hard set internally
		.setSurface(pSurface->mSurface.get())
		.setPreTransform(mSupport.capabilities.currentTransform)
		.setOldSwapchain(nullptr) // swap chain must be recreated if the window is resized. this is not yet handled
		// Determined above
		.setImageExtent(mResolution)
		.setImageFormat(mSurfaceFormat.format)
		.setImageColorSpace(mSurfaceFormat.colorSpace)
		.setPresentMode(mPresentationMode)
		.setMinImageCount(minImageCount)
		// TODO: These variables can be driven by SwapChainInfo
		.setImageArrayLayers(1) // always 1 unless developing a stereoscopic 3D application
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setClipped(true);

	// TODO: Assumes there is always a graphics and presentation queue. Can the information about what queues are actually in use go in QFG?
	auto allQueues = mQueueFamilyGroup.allQueues(); // need to retain in memory until after the vulkan device is deleted (setPQueueFamilyIndices doesnt copy the data)
	if (mQueueFamilyGroup.idxGraphicsQueue != mQueueFamilyGroup.idxPresentationQueue)
	{
		info.setImageSharingMode(vk::SharingMode::eConcurrent);
		info.setQueueFamilyIndexCount(2);
		info.setPQueueFamilyIndices(allQueues.data());
	}
	else
	{
		info.setImageSharingMode(vk::SharingMode::eExclusive);
		info.setQueueFamilyIndexCount(0);
		info.setPQueueFamilyIndices(nullptr);
	}

	mSwapChain = mpDevice->mDevice->createSwapchainKHRUnique(info);
}

std::vector<vk::Image> SwapChain::queryImages() const
{
	assert(mpDevice != nullptr);
	return mpDevice->mDevice->getSwapchainImagesKHR(mSwapChain.get());
}

std::vector<vk::UniqueImageView> SwapChain::createImageViews() const
{
	auto images = this->queryImages();
	uSize imageCount = images.size();
	auto views = std::vector<vk::UniqueImageView>(imageCount);
	for (uSize i = 0; i < imageCount; ++i)
	{
		views[i] = mpDevice->mDevice->createImageViewUnique(vk::ImageViewCreateInfo()
			.setImage(images[i])
			.setFormat(mSurfaceFormat.format)
			// TODO: These can be configured by something in the future
			.setViewType(vk::ImageViewType::e2D)
			.setComponents(vk::ComponentMapping()
				.setR(vk::ComponentSwizzle::eIdentity)
				.setG(vk::ComponentSwizzle::eIdentity)
				.setB(vk::ComponentSwizzle::eIdentity)
				.setA(vk::ComponentSwizzle::eIdentity)
			)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0).setLevelCount(1)
				.setBaseArrayLayer(0).setLayerCount(1)
			)
		);
	}
	return views;
}

void SwapChain::destroy()
{
	mpDevice = nullptr;
	mSwapChain.reset();
}
