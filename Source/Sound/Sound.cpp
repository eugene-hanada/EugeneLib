#include "../../Include/Sound/Sound.h"
#include "Xaudio2/Xa2Sound.h"

Eugene::Sound* sound = nullptr;

Eugene::Sound::~Sound()
{
}

Eugene::Sound::Sound()
{
}

Eugene::Sound* Eugene::CreateSound(void)
{
	if (sound != nullptr)
	{
		return sound;
	}
	return (sound = new Xa2Sound{});
}
