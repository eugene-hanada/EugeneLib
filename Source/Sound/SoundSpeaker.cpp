#include "../../Include/Sound/SoundSpeaker.h"

#ifdef USE_WINDOWS
#include "Xaudio2/Xa2SoundSpeaker.h"
#elif USE_ANDROID
#include "AAudio/AaSoundSpeaker.h"
#endif

Eugene::SoundSpeaker::SoundSpeaker(std::uint16_t inChannel, std::uint16_t outChannel,const float maxPitchRate):
		SoundBase{inChannel,outChannel},maxPitchRate_{maxPitchRate}
{
}

Eugene::SoundSpeaker::~SoundSpeaker()
{
}

