#include "../../Include/Sound/Sound.h"
#include "Xaudio2/Xa2Sound.h"
#include "../../Include/Common/EugeneLibException.h"

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
		throw CreateErrorException{"すでにSoundは生成されています"};
	}
	isCreate = true;
	return new Xa2Sound{};
}

Eugene::UniqueSound Eugene::CreateSoundUnique(void)
{
	return UniqueSound{ CreateSound()};
}
