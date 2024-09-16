//
// Created by eugen on 2024/04/14.
//

#include "../../../Include/Sound/AAudio/AaSoundSpeaker.h"
#include "../../../Include/Sound/AAudio/AaSubmix.h"
#include "../../../Include/Sound/SoundFile.h"
#include "../../../Include/Sound/SoundControl.h"

Eugene::SoundSpeaker::SoundSpeaker(AaSubmix*  submix, const SoundFile &soundFile,std::uint16_t outChannel):
        SoundBase{soundFile.GetFormat().channel, outChannel},voice_{static_cast<SoundBase&>(*this),submix,soundFile.GetFormat()}
{
}

Eugene::SoundSpeaker::~SoundSpeaker()
{

}

void Eugene::SoundSpeaker::Play(int loopCount)
{
    voice_.Play(loopCount);
}

void Eugene::SoundSpeaker::Stop(void)
{
    voice_.Stop();
}

bool Eugene::SoundSpeaker::IsEnd(void) const
{
    return voice_.IsEnd();
}

void Eugene::SoundSpeaker::SetPitchRate(float rate)
{

}

void Eugene::SoundSpeaker::SetVolume(float volume)
{
    volume_ = volume * volume;
}


void Eugene::SoundSpeaker::SetPan(std::span<float> volumes)
{
   voice_.SetOutMatrix(volumes);
}

void Eugene::SoundSpeaker::SetOutput(SoundControl& control)
{
    outChannel_ = control.GetInChannel();
    auto submix = reinterpret_cast<AaSubmix*>(control.Get());
    voice_.SetOut(submix, inChannel_,outChannel_);
}

void Eugene::SoundSpeaker::SetData(const std::uint8_t* ptr, const std::uint64_t size)
{
    voice_.SetData(ptr,size);
}