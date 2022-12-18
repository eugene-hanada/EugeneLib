#pragma once
#include "../../../Include/Graphics/DepthStencilViews.h"
#include "Dx12Views.h"

namespace EugeneLib
{
	class Dx12DepthStencilViews :
		public Dx12Views, public DepthStencilViews
	{
	public:
		Dx12DepthStencilViews(ID3D12Device* device, size_t size);
		void Create(GpuResource& resource, size_t idx) final;
	private:
		void* GetViews(void) const final;
	};
}
