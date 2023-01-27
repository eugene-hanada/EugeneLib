#include "../../Include/Graphics/RenderTargetViews.h"

#include "DirectX12/Dx12RenderTargetViews.h"

Eugene::RenderTargetViews::~RenderTargetViews()
{
}

const size_t Eugene::RenderTargetViews::GetSize(void)
{
	return size_;
}

Eugene::RenderTargetViews::RenderTargetViews(size_t size) :
	size_{size}
{
}
