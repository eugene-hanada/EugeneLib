#pragma once
#include "../../../Include/Graphics/RenderTargetViews.h"

namespace Eugene
{
	class VkRenderTargetViews :
		public RenderTargetViews
	{
	public:

	private:
		// RenderTargetViews を介して継承されました
		void Create(ImageResource& resource, std::uint64_t idx, const Format& format) final;
		void* GetViews(void) const final;

	};
}