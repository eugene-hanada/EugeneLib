#include "../../Include/Sound/SoundBase.h"


Eugene::SoundBase::SoundBase() :
	volume_{1.0f}, outChannel_{1u}
{
}

void Eugene::SoundBase::SetOutput(SoundControl& output)
{
	// 何もしない
}

Eugene::SoundBase::SoundBase(std::uint16_t inChannel, std::uint16_t outChannel) :
    inChannel_{inChannel}, outChannel_{outChannel},volume_{1.0f}
{

}
