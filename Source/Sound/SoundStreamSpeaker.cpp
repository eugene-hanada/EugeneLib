#include "../../Include/Sound/SoundStreamSpeaker.h"
#include "Xaudio2/Xa2SoundStreamSpeaker.h"

Eugene::SoundStreamSpeaker::SoundStreamSpeaker(std::uintptr_t devicePtr, const std::filesystem::path& path, std::uint16_t outChannel, float maxPitchRate) :
	maxPitchRate_{maxPitchRate}
{
	outChannel_ = outChannel;
	impl_ = std::make_unique<SoundStreamSpeakerImpl>(devicePtr, *this, path);
}

Eugene::SoundStreamSpeaker::~SoundStreamSpeaker()
{
}

void Eugene::SoundStreamSpeaker::SetVolume(float volume)
{
	impl_->SetVolume(volume);
}
void Eugene::SoundStreamSpeaker::SetPan(std::span<float> volumes)
{
	impl_->SetPan(volumes);
}

void Eugene::SoundStreamSpeaker::SetOutput(SoundControl& control)
{
	impl_->SetOutput(control);
}