#pragma once
#include "Dx12Views.h"
#include "../../../Include/Graphics/RenderTargetViews.h"

namespace Eugene
{

	class Dx12RenderTargetViews :
		public Dx12Views, public RenderTargetViews
	{
	public:
		Dx12RenderTargetViews(ID3D12Device* device, std::uint64_t size, bool isShaderVisible);
	private:
		void Create(ImageResource& resource, std::uint64_t idx, const Format& format) final;
		void* GetViews(void) const;
	};
}
