#include "../../Include/Sound/SoundControl.h"

#include "Xaudio2/Xa2SoundControl.h"

void* Eugene::SoundControl::Get(void)
{
	return impl_->Get();
}

void Eugene::SoundControl::SetVolume(float volume)
{
	impl_->SetVolume(volume);
}

void Eugene::SoundControl::SetPan(std::span<float> volumes)
{
	impl_->SetPan(volumes);
}

void Eugene::SoundControl::SetOutput(SoundControl& control)
{
	impl_->SetOutput(control);
}

Eugene::SoundControl::SoundControl(std::uintptr_t devicePtr, std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t stage)
{
	inChannel_ = inChannel;
	outChannel_ = outChannel;
	impl_ = std::make_unique<SoundControlImpl>(devicePtr, *this, sample, stage);
}
