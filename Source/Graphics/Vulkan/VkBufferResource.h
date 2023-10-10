#pragma once
#include "../../../Include/Graphics/BufferResource.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	class VkGraphics;
	class VkBufferResource :
		public BufferResource
	{
	public:
		VkBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size);
	private:

		// BufferResource ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		bool CanMap(void) const final;
		void* GetResource(void) final;
		std::uint64_t GetSize(void) final;
		vk::UniqueDeviceMemory memory_;
		vk::UniqueBuffer buffer_;

	};
}

