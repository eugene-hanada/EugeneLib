#include "../../Include/Sound/SoundBase.h"

Eugene::SoundBase::~SoundBase()
{
}

float Eugene::SoundBase::GetVolume(void) const
{
	return volume_;
}

std::uint16_t Eugene::SoundBase::GetOutChannel(void) const
{
	return outChannel_;
}

std::uint16_t Eugene::SoundBase::GetInChannel(void) const
{
	return inChannel_;
}

Eugene::SoundBase::SoundBase() :
	volume_{1.0f}, outChannel_{1u}
{
}

void Eugene::SoundBase::SetOutput(SoundControl& output)
{

}
