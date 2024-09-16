#include "../../../Include/Sound/AAudio/AaSoundControl.h"
#include "../../../Include/Sound/AAudio/resampler/MultiChannelResampler.h"
#include "../../../Include/Utils//EugeneLibException.h"

Eugene::SoundControl::SoundControl(AaSubmix*  submix,std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t stage) :
        SoundBase{inChannel,outChannel},submix_{*this,submix,sample}
{
}

Eugene::SoundControl::~SoundControl()
{
}

void Eugene::SoundControl::SetOutChannel(std::uint16_t channel)
{
	outChannel_ = channel;
}

void Eugene::SoundControl::SetPan(std::span<float> volumes)
{
	submix_.SetOutMatrix(volumes);
}

void Eugene::SoundControl::SetVolume(float volume)
{
	volume_ = volume * volume;
}

void Eugene::SoundControl::SetOutput(SoundControl& control)
{
    outChannel_ = control.GetInChannel();
    auto submix = static_cast<AaSubmix*>(control.Get());
    submix_.SetOut(submix, inChannel_,outChannel_);
}
