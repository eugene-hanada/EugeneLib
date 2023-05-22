#include "../../Include/Graphics/Graphics.h"

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

void Eugene::Graphics::ResizeBackBuffer(const Vector2& size)
{
}

void Eugene::Graphics::SetFullScreenFlag(bool isFullScreen)
{
}
