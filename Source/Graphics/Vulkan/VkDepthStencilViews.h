#pragma once
#include "../../../Include/Graphics/DepthStencilViews.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	class  VkDepthStencilView :
		public DepthStencilViews
	{
	public:
		VkDepthStencilView(const vk::Device& device, size_t size);
	private:
		// DepthStencilViews ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		void Create(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) final;

		std::vector<vk::UniqueImageView> imageViews_;
	};
}