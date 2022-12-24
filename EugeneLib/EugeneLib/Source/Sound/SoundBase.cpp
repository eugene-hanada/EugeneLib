#include "../../Include/Sound/SoundBase.h"

EugeneLib::SoundBase::~SoundBase()
{
}

float EugeneLib::SoundBase::GetVolume(void) const
{
	return volume_;
}

std::uint16_t EugeneLib::SoundBase::GetOututChannel(void) const
{
	return outChannel_;
}

EugeneLib::SoundBase::SoundBase() :
	volume_{1.0f}, outChannel_{1u}
{
}
