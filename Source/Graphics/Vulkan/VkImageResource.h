#pragma once
#include <vulkan/vulkan.hpp>
#include "../../../Include/Graphics/ImageResource.h"

namespace Eugene
{
	class VkImageResource :
		public ImageResource
	{
	public:
		VkImageResource(const vk::Device& device, const TextureInfo& info);
	private:
		vk::UniqueDeviceMemory memory_;
		vk::UniqueImage image_;
		vk::UniqueImageView imageView_;
	};
}
