#include "VkRenderTargetViews.h"
#include "VkImageResource.h"
#include "VkGraphics.h"

Eugene::VkRenderTargetViews::VkRenderTargetViews(std::uint64_t size) :
	RenderTargetViews{size}
{
	imageViews_.resize(size);
}

void Eugene::VkRenderTargetViews::Create(ImageResource& resource, std::uint64_t idx)
{
	if (imageViews_.size() <= idx)
	{
		return;
	}

	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };
	auto format = VkGraphics::FormatToVkFormat[static_cast<size_t>(resource.GetFormat())];
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(*data->image_);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(format);
	viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	viewInfo.subresourceRange.setLevelCount(data->mipmapLevels_);
	viewInfo.subresourceRange.setLayerCount(data->arraySize_);

	imageViews_[idx].imageView = data->image_.getOwner().createImageViewUnique(viewInfo);
	imageViews_[idx].size = resource.GetSize();
}

void* Eugene::VkRenderTargetViews::GetViews(void) 
{
	return &imageViews_;
}
