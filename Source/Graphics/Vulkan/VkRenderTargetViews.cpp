#include "../../../Include/Graphics/Vulkan/VkRenderTargetViews.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"

Eugene::RenderTargetViews::RenderTargetViews(std::uint32_t size)
{
	imageViews_.resize(size);
}

void Eugene::RenderTargetViews::Create(ImageResource& resource, std::uint32_t idx)
{
	if (imageViews_.size() <= idx)
	{
		return;
	}

	auto& data{ resource.GetResource() };
	auto format = Graphics::FormatToVkFormat[static_cast<std::size_t>(resource.GetFormat())];
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(*data.image_);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(format);
	viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	viewInfo.subresourceRange.setLevelCount(data.mipmapLevels_);
	viewInfo.subresourceRange.setLayerCount(data.arraySize_);

	imageViews_[idx].imageView = Graphics::GetInstance().device_->createImageViewUnique(viewInfo);
	imageViews_[idx].size = resource.GetSize();
}

