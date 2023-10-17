#pragma once
#include "../../../Include/Graphics/RenderTargetViews.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	class VkRenderTargetViews :
		public RenderTargetViews
	{
	public:
		VkRenderTargetViews(std::uint64_t size);
	private:
		// RenderTargetViews ����Čp������܂���
		void Create(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) final;
		std::vector<vk::UniqueImageView> imageViews_;
	};
}