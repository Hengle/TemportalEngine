#pragma once

#include "TemportalEnginePCH.hpp"

#include "types/integer.h"
#include "graphics/ShaderMetadata.hpp"
#include "graphics/VertexDescription.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.hpp>

NS_GRAPHICS
class LogicalDevice;

class ShaderModule
{
	friend class Pipeline;
	friend class VulkanApi;

public:

	ShaderModule();
	ShaderModule(ShaderModule &&other);
	~ShaderModule();
	ShaderModule& operator=(ShaderModule&& other);

	ShaderModule& setStage(vk::ShaderStageFlagBits stage);
	ShaderModule& setBinary(std::vector<ui32> binary);
	ShaderModule& setMetadata(graphics::ShaderMetadata metadata);

	/**
	 * Tells the shader module what the byte offset is for a given property from the data that will be sent for rendering.
	 * The shader must already have metadata initialized, which acts as the bridge from user defined structure to layout location.
	 */
	ShaderModule& setVertexDescription(VertexDescription desc);

	bool isLoaded() const;
	void create(LogicalDevice const *pDevice);
	void destroy();

private:
	vk::ShaderStageFlagBits mStage;
	std::vector<ui32> mBinary;
	graphics::ShaderMetadata mMetadata;
	std::string mMainOpName;
#ifndef NDEBUG
	std::unordered_map<std::string, uSize> mAttributeByteCount;
#endif

	vk::UniqueShaderModule mInternal;
	vk::VertexInputBindingDescription mBinding;
	std::unordered_map<std::string, vk::VertexInputAttributeDescription> mAttributes;

	vk::PipelineShaderStageCreateInfo getPipelineInfo() const;

};

NS_END
