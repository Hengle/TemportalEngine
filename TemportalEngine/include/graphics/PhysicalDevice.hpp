#pragma once

#include "TemportalEnginePCH.hpp"

#include "graphics/QueueFamilyGroup.hpp"
#include "graphics/SwapChainSupport.hpp"
#include "graphics/LogicalDevice.hpp"

#include <unordered_set>
#include <vulkan/vulkan.hpp>

NS_GRAPHICS
class VulkanInstance;
class Surface;
class LogicalDeviceInfo;

class PhysicalDevice
{
	friend class VulkanInstance;
	friend class VulkanApi;

public:
	PhysicalDevice();
	PhysicalDevice(PhysicalDevice const &other);

	void* get();

	vk::PhysicalDeviceProperties const getProperties() const;
	std::unordered_set<std::string> getSupportedExtensionNames() const;
	vk::PhysicalDeviceFeatures const getFeatures() const;
	QueueFamilyGroup queryQueueFamilyGroup() const;
	SwapChainSupport querySwapChainSupport() const;
	vk::PhysicalDeviceMemoryProperties getMemoryProperties() const;

	void invalidate();

	LogicalDevice createLogicalDevice(LogicalDeviceInfo const *pInfo) const;

private:
	graphics::Surface const *mpSurface;
	vk::PhysicalDevice mDevice;

	PhysicalDevice(vk::PhysicalDevice &device, graphics::Surface const *pSurface);
	vk::SurfaceKHR getVulkanSurface() const;

};

NS_END
