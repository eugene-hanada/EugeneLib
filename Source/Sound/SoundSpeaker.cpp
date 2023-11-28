#include "../../Include/Sound/SoundSpeaker.h"


#include "Xaudio2/Xa2SoundSpeaker.h"

Eugene::SoundSpeaker::SoundSpeaker(std::uintptr_t devicePtr, const SoundFile& soundFile, std::uint16_t outChannel,const float maxPitchRate):
	maxPitchRate_{maxPitchRate}
{
	outChannel_ = outChannel;
	impl_ = std::make_unique<SoundSpeakerImpl>(devicePtr, *this, soundFile);
}

void Eugene::SoundSpeaker::Play(int loopCount)
{
	impl_->Play(loopCount);
}

void Eugene::SoundSpeaker::Stop(void)
{
	impl_->Stop();
}

bool Eugene::SoundSpeaker::IsEnd(void) const
{
	return impl_->IsEnd();
}

void Eugene::SoundSpeaker::SetPitchRate(float rate)
{
	impl_->SetPitchRate(rate);
}

void Eugene::SoundSpeaker::SetData(const std::uint8_t* ptr, const std::uint64_t size)
{
	impl_->SetData(ptr, size);
}

Eugene::SoundSpeaker::~SoundSpeaker()
{
}

void Eugene::SoundSpeaker::SetVolume(float volume)
{
	impl_->SetVolume(volume);
}

void Eugene::SoundSpeaker::SetPan(std::span<float> volumes)
{
	impl_->SetPan(volumes);
}

void Eugene::SoundSpeaker::SetOutput(SoundControl& control)
{
	impl_->SetOutput(control);
}