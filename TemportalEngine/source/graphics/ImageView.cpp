#include "graphics/ImageView.hpp"

#include "graphics/LogicalDevice.hpp"
#include "graphics/Image.hpp"

using namespace graphics;

ImageView::ImageView(ImageView &&other)
{
	*this = std::move(other);
}

ImageView& ImageView::operator=(ImageView &&other)
{
	this->mImage = other.mImage;
	this->mFormat = other.mFormat;
	this->mViewType = other.mViewType;
	this->mCompMapping = other.mCompMapping;
	this->mSubresourceRange = other.mSubresourceRange;
	this->mInternal = std::move(other.mInternal);
	return *this;
}

ImageView::~ImageView()
{
	this->mInternal.reset();
}

ImageView& ImageView::setImage(Image *image)
{
	this->setRawImage(*reinterpret_cast<vk::Image*>(image->get()));
	//this->setFormat(image)
	return *this;
}

ImageView& ImageView::setRawImage(vk::Image const &image)
{
	this->mImage = image;
	return *this;
}

ImageView& ImageView::setFormat(vk::Format format)
{
	this->mFormat = format;
	return *this;
}

ImageView& ImageView::setViewType(vk::ImageViewType type)
{
	this->mViewType = type;
	return *this;
}

ImageView& ImageView::setComponentMapping(vk::ComponentMapping mapping)
{
	this->mCompMapping = mapping;
	return *this;
}

ImageView& ImageView::setRange(vk::ImageSubresourceRange range)
{
	this->mSubresourceRange = range;
	return *this;
}

ImageView& ImageView::create(LogicalDevice const *device)
{
	this->mInternal = device->mDevice->createImageViewUnique(
		vk::ImageViewCreateInfo()
		.setImage(this->mImage)
		.setFormat(this->mFormat)
		.setViewType(this->mViewType)
		.setComponents(this->mCompMapping)
		.setSubresourceRange(this->mSubresourceRange)
	);
	return *this;
}
