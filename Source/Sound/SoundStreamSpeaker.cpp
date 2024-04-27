#include "../../Include/Sound/SoundStreamSpeaker.h"

#ifdef USE_WINDOWS
#include "Xaudio2/Xa2SoundStreamSpeaker.h"
#elif USE_ANDROID
#include "AAudio/AaSoundStreamSpeaker.h"
#endif

Eugene::SoundStreamSpeaker::SoundStreamSpeaker(std::uint16_t outChannel, float maxPitchRate) :
	maxPitchRate_{maxPitchRate}
{
	outChannel_ = outChannel;
	//impl_ = std::make_unique<SoundStreamSpeakerImpl>(devicePtr, *this, path);
}

Eugene::SoundStreamSpeaker::~SoundStreamSpeaker()
{
}

//void Eugene::SoundStreamSpeaker::SetVolume(float volume)
//{
//	impl_->SetVolume(volume);
//}
//void Eugene::SoundStreamSpeaker::SetPan(std::span<float> volumes)
//{
//	impl_->SetPan(volumes);
//}
//
//void Eugene::SoundStreamSpeaker::SetOutput(SoundControl& control)
//{
//	impl_->SetOutput(control);
//}