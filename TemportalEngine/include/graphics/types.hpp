#pragma once

#include "TemportalEnginePCH.hpp"

namespace vk
{
	struct PhysicalDeviceFeatures;
};

NS_GRAPHICS
struct SwapChainSupport;

struct PhysicalDeviceProperties
{
	/**
	 * Wrapper of `vk::PhysicalDeviceType`
	 */
	struct Type
	{
		enum class Enum : ui8
		{
			eOther = 0,
			eIntegratedGpu = 1,
			eDiscreteGpu = 2,
			eVirtualGpu = 3,
			eCpu = 4,
		};
		static std::vector<Enum> ALL;
		static std::string to_string(Enum value);
	};
	struct Extension
	{
		typedef std::string Type;
		static Type SwapChain;
		static std::vector<Type> ALL;
	};
	/**
	 * Wrapper of `vk::PhysicalDeviceType`
	 */
	struct Feature
	{
		enum class Enum : ui8
		{
			RobustBufferAccess,
			FullDrawIndex,
			ImageCubeArray,
			IndependentBlend,
			GeometryShader,
			TessellationShader,
			SampleRateShading,
			DualSrcBlend,
			LogicOp,
			MultiDrawIndirect,
			DrawIndirectFirstInstance,
			DepthClamp,
			DepthBiasClamp,
			FillModeNonSolid,
			DepthBounds,
			WideLines,
			LargePoints,
			AlphaToOne,
			MultiViewport,
			SamplerAnisotropy,
			TextureCompressionETC2,
			TextureCompressionASTC_LDR,
			TextureCompressionBC,
			OcclusionQueryPrecise,
			PipelineStatisticsQuery,
			VertexPipelineStoresAndAtomics,
			FragmentStoresAndAtomics,
			ShaderTessellationAndGeometryPointSize,
			ShaderImageGatherExtended,
			ShaderStorageImageExtendedFormats,
			ShaderStorageImageMultisample,
			ShaderStorageImageReadWithoutFormat,
			ShaderStorageImageWriteWithoutFormat,
			ShaderUniformBufferArrayDynamicIndexing,
			ShaderSampledImageArrayDynamicIndexing,
			ShaderStorageBufferArrayDynamicIndexing,
			ShaderStorageImageArrayDynamicIndexing,
			ShaderClipDistance,
			ShaderCullDistance,
			ShaderFloat64,
			ShaderInt64,
			ShaderInt16,
			ShaderResourceResidency,
			ShaderResourceMinLod,
			SparseBinding,
			SparseResidencyBuffer,
			SparseResidencyImage2D,
			SparseResidencyImage3D,
			SparseResidency2Samples,
			SparseResidency4Samples,
			SparseResidency8Samples,
			SparseResidency16Samples,
			SparseResidencyAliased,
			VariableMultisampleRate,
		};
		static std::vector<Enum> ALL;
		static std::string to_string(Enum value);
		static bool hasFeature(vk::PhysicalDeviceFeatures const *features, Enum type);
		static bool enableFeature(vk::PhysicalDeviceFeatures *features, Enum type);
	};
};

struct QueueFamily
{
	enum class Enum : ui8
	{
		eGraphics,
		ePresentation,
	};
	static std::vector<Enum> ALL;
	static std::string to_string(Enum value);
};

struct SwapChainSupportType
{
	enum class Enum : ui8
	{
		eHasAnySurfaceFormat,
		eHasAnyPresentationMode,
	};
	static std::vector<Enum> ALL;
	static std::string to_string(Enum value);
	static bool hasSupport(SwapChainSupport *support, Enum type);
};

NS_END
