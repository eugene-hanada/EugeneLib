#include "../../Include/Sound/Sound.h"

#ifdef EUGENE_WINDOWS
#include "Xaudio2/Xa2Sound.h"
#elif USE_ANDROID
#include "AAudio/AaSound.h"
#endif
#include "../../Include/Utils/EugeneLibException.h"
namespace {
	bool isCreate = false;
}

Eugene::Sound::~Sound()
{
	isCreate = false;
}

Eugene::Sound::Sound() :
	sampleRate_{0u}
{
}


Eugene::Sound* Eugene::CreateSound(void)
{
	if (isCreate)
	{
		throw EugeneLibException{"すでにSoundは生成されています"};
	}
	isCreate = true;
#ifdef EUGENE_WINDOWS
	return new Xaudio2Sound{};
#elif EUGENE_ANDROID
    return new AaudioSound{};
#endif
}

Eugene::UniqueSound Eugene::CreateSoundUnique(void)
{
	return UniqueSound{ CreateSound()};
}
