#include "../../Include/Sound/Sound.h"
#include "Xaudio2/Xa2Sound.h"
#include "../../Include/Common/EugeneLibException.h"

namespace {
	bool isCreate = false;
}

Eugene::Sound::~Sound()
{
}

Eugene::Sound::Sound() :
	sampleRate_{0u}
{
}

Eugene::Sound* Eugene::CreateSound(void)
{
	if (isCreate)
	{
		throw CreateErrorException{"すでにSoundは生成されています"};
	}
	isCreate = false;
	return new Xa2Sound{};
}

Eugene::SoundUnique Eugene::CreateSoundUnique(void)
{
	return SoundUnique{ new Xa2Sound{} };
}
