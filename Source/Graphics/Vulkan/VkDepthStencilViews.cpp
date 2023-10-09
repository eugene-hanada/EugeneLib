#include "VkDepthStencilViews.h"
#include "VkImageResource.h"
#include "VkGraphics.h"

Eugene::VkDepthStencilView::VkDepthStencilView(const vk::Device& device, size_t size) :
	DepthStencilViews{size}
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
	imageViews_[idx] = data->image_.getOwner().createImageViewUnique(info);
}

void* Eugene::VkDepthStencilView::GetViews(void)
{
	return (imageViews_.data());
}
