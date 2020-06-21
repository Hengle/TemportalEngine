#pragma once

#include "TemportalEnginePCH.hpp"

#include "math/Vector.hpp"

#include <vulkan/vulkan.hpp>

NS_GRAPHICS
class LogicalDevice;

class ImageSampler
{

public:
	ImageSampler();
	~ImageSampler();

	ImageSampler& setFilter(vk::Filter magnified, vk::Filter minified);
	ImageSampler& setAddressMode(math::Vector<ui8, 3> uvwMode);
	ImageSampler& setAnistropy(std::optional<f32> anistropy);
	ImageSampler& setBorderColor(vk::BorderColor colorSetting);
	ImageSampler& setNormalizeCoordinates(bool enabled);
	ImageSampler& setCompare(std::optional<vk::CompareOp> compareOp);
	ImageSampler& setMipLOD(vk::SamplerMipmapMode mode, f32 bias, math::Vector2 range);

	ImageSampler& create(LogicalDevice *device);
	void invalidate();

private:
	vk::Filter mFilterMag, mFilterMin;
	math::Vector<ui8, 3> mAddressModes;
	std::optional<f32> mAnistropy;
	vk::BorderColor mBorderColor;
	bool mbNormalizeCoordinates;
	std::optional<vk::CompareOp> mCompareOp;
	vk::SamplerMipmapMode mMipLODMode;
	f32 mMipLODBias;
	math::Vector2 mMipLODRange;

	vk::UniqueSampler mInternal;

};

NS_END
