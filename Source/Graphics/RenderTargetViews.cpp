#include "../../Include/Graphics/RenderTargetViews.h"

#include "DirectX12/Dx12RenderTargetViews.h"

Eugene::RenderTargetViews::~RenderTargetViews()
{
}

const std::uint64_t Eugene::RenderTargetViews::GetSize(void)
{
	return size_;
}

Eugene::RenderTargetViews::RenderTargetViews(std::uint64_t size) :
	size_{size}
{
}
