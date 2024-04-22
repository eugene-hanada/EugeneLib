#include "../../Include/Sound/SoundControl.h"

#ifdef USE_WINDOWS
#include "Xaudio2/Xa2SoundControl.h"
#elif USE_ANDROID
#include "AAudio/AaSoundControl.h"
#endif
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
	impl_.reset(new SoundControlImpl(devicePtr, *this, sample, stage));
}

void Eugene::SoundControl::SoundControlImplDeleter::operator()(
Eugene::SoundControl::SoundControlImpl *ptr)
{
    delete ptr;
}
