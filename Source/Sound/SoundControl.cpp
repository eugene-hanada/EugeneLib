#include "../../Include/Sound/SoundControl.h"


Eugene::SoundControl::SoundControl( std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t stage)
{
	inChannel_ = inChannel;
	outChannel_ = outChannel;
	//impl_.reset(new SoundControlImpl(devicePtr, *this, sample, stage));
}
