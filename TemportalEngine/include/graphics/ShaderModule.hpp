#pragma once

#include "TemportalEnginePCH.hpp"

#include <optional>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

NS_GRAPHICS
class LogicalDevice;

// TODO: This class is a great base template as a starting point for abstracting out information about binary assets and serialization
// readBinary is already implmented and could be abstracted out to a super class.
// Could even add converter methods to read a binary file and write as plain text or vice-versa.
// TODO: Can integrate https://github.com/google/shaderc for compiling shaders within the editor
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
	ShaderModule& setSource(std::string fileName);

	bool isLoaded() const;
	void create(LogicalDevice const *pDevice);
	void destroy();

private:
	std::string mFileName;
	std::string mMainOpName;
	vk::ShaderStageFlagBits mStage;
	vk::UniqueShaderModule mShader;

	std::optional<std::vector<char>> readBinary() const;
	vk::PipelineShaderStageCreateInfo getPipelineInfo() const;

};

NS_END
