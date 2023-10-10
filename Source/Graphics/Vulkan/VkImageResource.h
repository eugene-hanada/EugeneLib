#pragma once
#include <vulkan/vulkan.hpp>
#include "../../../Include/Graphics/ImageResource.h"

namespace Eugene
{
	class VkGraphics;
	class VkImageResource :
		public ImageResource
	{
	public:
		VkImageResource(const VkGraphics& graphics,const vk::Device& device, const TextureInfo& info);
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const Vector2I& size, float clearValue);

		struct Data
		{
			vk::UniqueDeviceMemory memory_;
			vk::UniqueImage image_;
			std::uint32_t arraySize_;
			std::uint32_t mipmapLevels_;
		};

	private:
		// ImageResource ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		bool CanMap(void) const final;
		void* GetResource(void) final;
		Vector2I GetSize(void) final;

		Data data_;
		Vector2I size_;
	};
}
