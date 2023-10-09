#pragma once
#include "../../../Include/Graphics/DepthStencilViews.h"
#include "Dx12Views.h"

namespace Eugene
{
	class Dx12DepthStencilViews :
		public Dx12Views, public DepthStencilViews
	{
	public:
		Dx12DepthStencilViews(ID3D12Device* device, std::uint64_t size);
		void Create(ImageResource& resource, std::uint64_t idx) final;
	private:
		void* GetViews(void) final;
	};
}
