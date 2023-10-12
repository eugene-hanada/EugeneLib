#include "VkImageResource.h"
#include "VkGraphics.h"

Eugene::VkImageResource::VkImageResource(const VkGraphics& graphics,const vk::Device& device, const TextureInfo& info) :
	ImageResource{info.format }
{
	auto format{ VkGraphics::FormatToVkFormat[static_cast<size_t>(info.format)] };
	vk::ImageCreateInfo imageInfo{};
	imageInfo.setArrayLayers(info.arraySize);
	imageInfo.setMipLevels(info.mipLevels);
	imageInfo.extent.setHeight(info.height);
	imageInfo.extent.setWidth(info.width);
	imageInfo.extent.setDepth(1u);
	imageInfo.setImageType(vk::ImageType::e2D);
	imageInfo.setTiling(vk::ImageTiling::eOptimal);
	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
	imageInfo.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
	imageInfo.setSamples(vk::SampleCountFlagBits::e1);
	imageInfo.setFormat(format);

	// Image生成
	data_.image_ = device.createImageUnique(imageInfo);

	// DeviceMemory生成
	data_.memory_ = graphics.CreateMemory(data_.image_);

	// バインド
	device.bindImageMemory(*data_.image_, *data_.memory_, 0);

	data_.arraySize_ = info.arraySize;
	data_.mipmapLevels_ = info.mipLevels;
}

Eugene::VkImageResource::VkImageResource(const VkGraphics& graphics, const vk::Device& device, const Vector2I& size, float clearValue) :
	ImageResource{Format::D32_FLOAT}
{
	constexpr auto format{ vk::Format::eD32Sfloat };
	vk::ImageCreateInfo imageInfo{};
	imageInfo.setArrayLayers(1);
	imageInfo.setMipLevels(1);
	imageInfo.extent.setHeight(size.y);
	imageInfo.extent.setWidth(size.x);
	imageInfo.extent.setDepth(1u);
	imageInfo.setTiling(vk::ImageTiling::eOptimal);
	imageInfo.setImageType(vk::ImageType::e2D);
	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
	imageInfo.setUsage(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eDepthStencilAttachment);
	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
	imageInfo.setSamples(vk::SampleCountFlagBits::e1);
	imageInfo.setFormat(format);


	// Image生成
	data_.image_ = device.createImageUnique(imageInfo);

	// DeviceMemory生成
	data_.memory_ = graphics.CreateMemory(data_.image_);

	// バインド
	device.bindImageMemory(*data_.image_, *data_.memory_, 0);

	data_.arraySize_ = 1;
	data_.mipmapLevels_ = 1;
}

bool Eugene::VkImageResource::CanMap(void) const
{
	return false;
}

void* Eugene::VkImageResource::GetResource(void)
{
	return static_cast<void*>(&data_);
}

Eugene::Vector2I Eugene::VkImageResource::GetSize(void)
{
	
	return size_;
}
