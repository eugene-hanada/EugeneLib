#pragma once
#include "../../../Include/Graphics/RenderTargetViews.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Math/Geometry.h"

namespace Eugene
{
	class VkRenderTargetViews :
		public RenderTargetViews
	{
	public:
		
		VkRenderTargetViews(std::uint64_t size);

		struct Data
		{
			vk::UniqueImageView image;
			Vector2I size;
		};
		using ViewsType = std::vector<Data>;
	private:
		// RenderTargetViews を介して継承されました
		void Create(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) final;
		ViewsType imageViews_;
	};
}