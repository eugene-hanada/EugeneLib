//
// Created by eugen on 2024/04/14.
//

#include "AaSoundSpeaker.h"
#include "AaSubmix.h"
#include "../../../Include/Sound/SoundFile.h"
#include "../../../Include/Sound/SoundControl.h"

Eugene::AaudioSpeaker::AaudioSpeaker(AaSubmix*  submix, const SoundFile &soundFile,std::uint16_t outChannel):
        SoundSpeaker{soundFile.GetFormat().channel,outChannel,2.0f},
        voice_{static_cast<SoundBase&>(*this),submix,soundFile.GetFormat()}
{
}

Eugene::AaudioSpeaker::~AaudioSpeaker()
{

}

void Eugene::AaudioSpeaker::Play(int loopCount)
{
    voice_.Play(loopCount);
}

void Eugene::AaudioSpeaker::Stop(void)
{
    voice_.Stop();
}

bool Eugene::AaudioSpeaker::IsEnd(void) const
{
    return voice_.IsEnd();
}

void Eugene::AaudioSpeaker::SetPitchRate(float rate)
{

}

void Eugene::AaudioSpeaker::SetVolume(float volume)
{
    volume_ = volume * volume;
}


void Eugene::AaudioSpeaker::SetPan(std::span<float> volumes)
{
   voice_.SetOutMatrix(volumes);
}

void Eugene::AaudioSpeaker::SetOutput(SoundControl& control)
{
    outChannel_ = control.GetInChannel();
    auto submix = reinterpret_cast<AaSubmix*>(control.Get());
    voice_.SetOut(submix, inChannel_,outChannel_);
}

void Eugene::AaudioSpeaker::SetData(const std::uint8_t* ptr, const std::uint64_t size)
{
    voice_.SetData(ptr,size);
}


AaVoice *Eugene::AaudioSpeaker::GetVoice() {
    return &voice_;
}
