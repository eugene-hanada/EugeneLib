#include "../../../Include/Graphics/Vulkan/VkGraphics.h"
#include "../../../Include/Graphics/Vulkan/VkDepthStencilViews.h"

Eugene::DepthStencilViews::DepthStencilViews(std::uint32_t size)
{
	imageViews_.resize(size);
}

void Eugene::DepthStencilViews::Create(ImageResource& resource, std::uint32_t idx)
{
	if (idx >= imageViews_.size())
	{
		return;
	}
	
	vk::ImageViewCreateInfo info{};
	info.setFormat(Graphics::FormatToVkFormat[static_cast<size_t>(resource.GetFormat())]);
	info.setImage(*resource.GetResource().image_);
	info.setViewType(vk::ImageViewType::e2D);
	info.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
	info.subresourceRange.setLayerCount(1u);
	info.subresourceRange.setLevelCount(1u);
	imageViews_[idx] = Graphics::GetInstance().device_->createImageViewUnique(info);
}
