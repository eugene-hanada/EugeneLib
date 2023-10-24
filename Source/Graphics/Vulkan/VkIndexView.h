#pragma once
#include "../../../Include/Graphics/IndexView.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Graphics/GraphicsCommon.h"

namespace Eugene
{
	class GpuResource;
	class VkIndexView :
		public IndexView
	{
	public:
		VkIndexView(std::uint32_t size, std::uint32_t indexNum, vk::Buffer& buffer);
	private:
		// IndexView ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		void* GetView(void) final;
	
		vk::Buffer buffer_;
	};
}