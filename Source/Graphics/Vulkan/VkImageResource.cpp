#include "VkImageResource.h"

Eugene::VkImageResource::VkImageResource(const vk::Device& device, const TextureInfo& info) :
	ImageResource{info.format }
{
	
}
