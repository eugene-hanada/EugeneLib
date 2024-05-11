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
}

Eugene::SoundStreamSpeaker::~SoundStreamSpeaker()
{
}
