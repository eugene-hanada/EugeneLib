#pragma once
#include "../../../Include/Graphics/BufferResource.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	struct VkBufferData
	{
		vk::UniqueDeviceMemory memory_;
		vk::UniqueBuffer buffer_;
	};

	class VkGraphics;
	class Image;
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
		VkBufferData data_;
	};

	class VkUploadableBufferResource :
		public BufferResource
	{
	public:
		VkUploadableBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size);
		VkUploadableBufferResource(const vk::Device& device, const VkGraphics& graphics, Image& image);
	private:
		// BufferResource ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		bool CanMap(void) const final;
		void* GetResource(void) final;
		std::uint64_t GetSize(void) final;
		void* Map(void) final;
		virtual void UnMap(void) final;
		VkBufferData data_;
	};
}

