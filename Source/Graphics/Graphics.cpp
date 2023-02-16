#include "../../Include/Graphics/Graphics.h"
#include "../../Source/Graphics/DirectX12/Dx12Graphics.h"

Eugene::Graphics::~Graphics()
{
}

Eugene::Graphics::Graphics()
{
}

Eugene::ImageResource& Eugene::Graphics::GetBackBufferResource(void)
{
    return GetBackBufferResource(GetNowBackBufferIndex());
}
