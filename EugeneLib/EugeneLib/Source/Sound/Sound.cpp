#include "../../Include/Sound/Sound.h"
#include "Xaudio2/Xa2Sound.h"

EugeneLib::Sound::~Sound()
{
}

EugeneLib::Sound::Sound()
{
}

EugeneLib::Sound* EugeneLib::CreateSound(void)
{
	return new Xa2Sound{};
}
