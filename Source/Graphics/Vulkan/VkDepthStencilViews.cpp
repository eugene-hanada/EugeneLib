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
	info.setFormat(Graphics::FormatToBackEndFormat(resource.GetFormat()));
	info.setImage(*static_cast<ImageResource::ImageData*>(resource.GetResource())->image_);
	info.setViewType(vk::ImageViewType::e2D);
	if (info.format == vk::Format::eD24UnormS8Uint)
	{
		// DepthとStencilの両方を使うので設定
		info.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
	}
	else if (resource.GetFormat() == Format::D32_FLOAT || resource.GetFormat() == Format::D16_UNORM)
	{
		// Depthのみを使うので設定
		info.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
	}
	else
	{
		// それ以外はとりあえずステンシルにしとく
		info.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eStencil);
	}

	info.subresourceRange.setLayerCount(1u);
	info.subresourceRange.setLevelCount(1u);
	imageViews_[idx] = Graphics::GetInstance().device_->createImageViewUnique(info);
}
