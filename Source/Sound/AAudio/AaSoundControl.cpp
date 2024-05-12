#include "AaSoundControl.h"
#include "resampler/MultiChannelResampler.h"
#include "../../../Include/Utils//EugeneLibException.h"

Eugene::AaudioControl::AaudioControl(AaSubmix*  submix,std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t stage) :
        SoundControl{sample,inChannel,outChannel,stage} ,submix_{*this,submix,sample}
{

}

Eugene::AaudioControl::~AaudioControl()
{
}

void* Eugene::AaudioControl::Get(void)
{
	return &submix_;
}

void Eugene::AaudioControl::SetOutChannel(std::uint16_t channel)
{
	outChannel_ = channel;
}

void Eugene::AaudioControl::SetPan(std::span<float> volumes)
{
	submix_.SetOutMatrix(volumes);
}

void Eugene::AaudioControl::SetVolume(float volume)
{
	volume_ = volume * volume;
}

void Eugene::AaudioControl::SetOutput(SoundControl& control)
{
    outChannel_ = control.GetInChannel();
    auto submix = static_cast<AaSubmix*>(control.Get());
    submix_.SetOut(submix, inChannel_,outChannel_);
}
