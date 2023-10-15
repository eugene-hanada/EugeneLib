#pragma once
#include "../../../Include/Graphics/RenderTargetViews.h"

namespace Eugene
{
	class VkRenderTargetViews :
		public RenderTargetViews
	{
	public:

	private:
		// RenderTargetViews ����Čp������܂���
		void Create(ImageResource& resource, std::uint64_t idx, const Format& format) final;
		void* GetViews(void) const final;

	};
}