#include "../../Include/Sound/Sound.h"
#include "Xaudio2/Xa2Sound.h"

EugeneLib::Sound* sound = nullptr;

EugeneLib::Sound::~Sound()
{
}

EugeneLib::Sound::Sound()
{
}

EugeneLib::Sound* EugeneLib::CreateSound(void)
{
	if (sound != nullptr)
	{
		return sound;
	}
	return (sound = new Xa2Sound{});
}
