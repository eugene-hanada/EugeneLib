#pragma once
#include "Dx12Views.h"
#include "../../../Include/Graphics/RenderTargetViews.h"

namespace EugeneLib
{

	class Dx12RenderTargetViews :
		public Dx12Views, public RenderTargetViews
	{
	public:
		Dx12RenderTargetViews(Graphics& graphics, size_t size, bool isShaderVisible);
	private:
		void Create(GpuResource& resource, size_t idx, const Format& format) final;
		void* GetViews(void) const;
	};
}
