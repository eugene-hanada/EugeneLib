#include "../../Include/Graphics/Graphics.h"

Eugene::EffekseerWarpper::~EffekseerWarpper()
{
}

Eugene::EffekseerWarpper::EffekseerWarpper()
{
}

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
