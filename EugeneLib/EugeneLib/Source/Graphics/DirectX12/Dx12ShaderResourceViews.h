#pragma once
#include "../../../Include/Graphics/ShaderResourceViews.h"
#include "Dx12Views.h"

namespace EugeneLib
{
	class Dx12ShaderResourceViews :
		public Dx12Views, public ShaderResourceViews
	{
	public:
		Dx12ShaderResourceViews(Graphics& graphics, size_t size);
		void CreateTexture(GpuResource& resource, size_t idx) final;
		void CreateConstantBuffer(GpuResource& resource, size_t idx) final;
		void* GetViews(void) const final;
	protected:
	};
}
