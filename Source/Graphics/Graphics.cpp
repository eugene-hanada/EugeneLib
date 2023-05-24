#include "../../Include/Graphics/Graphics.h"

#ifdef USE_EFFEKSEER
Eugene::EffekseerWarpper::~EffekseerWarpper()
{
}

Eugene::EffekseerWarpper::EffekseerWarpper()
{
}

#endif

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
