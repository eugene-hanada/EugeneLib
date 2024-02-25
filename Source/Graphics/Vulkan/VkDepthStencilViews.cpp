#include "VkDepthStencilViews.h"
#include "VkImageResource.h"
#include "VkGraphics.h"

Eugene::VkDepthStencilView::VkDepthStencilView(const vk::Device& device, size_t size) :
	DepthStencilViews{size}, device_{device}
{
	imageViews_.resize(size);
}

void Eugene::VkDepthStencilView::Create(ImageResource& resource, std::uint64_t idx)
{
	if (idx >= size_)
	{
		return;
	}
	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };
	vk::ImageViewCreateInfo info{};
	info.setFormat(VkGraphics::FormatToVkFormat[static_cast<size_t>(resource.GetFormat())]);
	info.setImage(*data->image_);
	info.setViewType(vk::ImageViewType::e2D);
	info.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
	info.subresourceRange.setLayerCount(1u);
	info.subresourceRange.setLevelCount(1u);
	imageViews_[idx] = device_.createImageViewUnique(info);
}

void* Eugene::VkDepthStencilView::GetViews(void)
{
	return (&imageViews_);
}
