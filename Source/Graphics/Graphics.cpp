#include "../../Include/Graphics/Graphics.h"

Eugene::Format Eugene::Graphics::backBufferFormat_{Format::NON};

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

Eugene::Graphics::Graphics():
    multiSampleCount_{0}
{
}

Eugene::ImageResource& Eugene::Graphics::GetBackBufferResource(void)
{
    return GetBackBufferResource(GetNowBackBufferIndex());
}

void Eugene::Graphics::ResizeBackBuffer(const glm::vec2& size, void* window)
{
}

void Eugene::Graphics::SetFullScreenFlag(bool isFullScreen)
{
}

const std::uint8_t Eugene::Graphics::GetMaxMultiSampleCount() const
{
    return multiSampleCount_;
}