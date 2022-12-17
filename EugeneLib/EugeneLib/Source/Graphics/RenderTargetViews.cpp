#include "../../Include/Graphics/RenderTargetViews.h"

#include "DirectX12/Dx12RenderTargetViews.h"

EugeneLib::RenderTargetViews::~RenderTargetViews()
{
}

const size_t EugeneLib::RenderTargetViews::GetSize(void)
{
	return size_;
}

EugeneLib::RenderTargetViews::RenderTargetViews(size_t size) :
	size_{size}
{
}
