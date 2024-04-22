//
// Created by eugen on 2024/04/14.
//

#include "AaSoundSpeaker.h"
#include "AaSubmix.h"
#include "../../../Include/Sound/SoundFile.h"
#include "../../../Include/Sound/SoundControl.h"

Eugene::SoundSpeaker::SoundSpeakerImpl::SoundSpeakerImpl(std::uintptr_t device, SoundSpeaker& speaker, const SoundFile& soundFile):
        AaVoice{reinterpret_cast<AaSubmix*>(device),
                soundFile.GetFormat().channel,speaker.outChannel_,
                soundFile.GetFormat().sample},speaker_{speaker},
                dataPtr_{nullptr},dataSize_{0ull},loopCount_{0},playBytye_{0ull}
{
    speaker_.inChannel_ = soundFile.GetFormat().channel;
    auto sample = soundFile.GetFormat().sample;
    byteParFrame_ = (sample * (speaker_.inChannel_ * soundFile.GetFormat().size) / 8ull) / sample;

    getNextFrameFunc_ = &SoundSpeakerImpl::GetPcm16NextFrame;
    if (soundFile.GetFormat().size == 32)
    {
        getNextFrameFunc_ = &SoundSpeakerImpl::GetPcm32NextFrame;
    }

}

Eugene::SoundSpeaker::SoundSpeakerImpl::~SoundSpeakerImpl()
{

}

void Eugene::SoundSpeaker::SoundSpeakerImpl::Play(int loopCount)
{
    loopCount_ = loopCount;
    playBytye_ = 0;
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::Stop(void)
{

}

bool Eugene::SoundSpeaker::SoundSpeakerImpl::IsEnd(void) const
{
    return false;
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::SetPitchRate(float rate)
{

}

void Eugene::SoundSpeaker::SoundSpeakerImpl::SetVolume(float volume)
{
    speaker_.volume_ = volume * volume;
}


void Eugene::SoundSpeaker::SoundSpeakerImpl::SetPan(std::span<float> volumes)
{
    std::copy(volumes.begin(),volumes.end(),outMatrix_.begin());
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::SetOutput(SoundControl& control)
{
    speaker_.outChannel_ = control.GetInChannel();
    auto submix = reinterpret_cast<AaSubmix*>(control.Get());
    SetOut(submix, speaker_.inChannel_,speaker_.outChannel_);
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::SetData(const std::uint8_t* ptr, const std::uint64_t size)
{
    dataPtr_ = (std::uint8_t*)ptr;
    dataSize_ = size;
    playBytye_ = dataSize_;
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::GetNextFrame(std::span<float> outSpan) {
    auto size = dataSize_ - playBytye_;
    if (size <= 0)
    {
        if (loopCount_ == 0) {
            return;
        }

        if (loopCount_ != -1)
        {
            loopCount_--;
        }
        playBytye_ = 0ull;
    }

    (this->*getNextFrameFunc_)(outSpan,size);

    playBytye_ += byteParFrame_;
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::GetPcm16NextFrame(
        std::span<float> outSpan,
       std::uint64_t size)
{
    std::span<std::int16_t> now{
        reinterpret_cast<std::int16_t*>(dataPtr_ + playBytye_),
        static_cast<std::size_t>(size /2ull )
    };
    for (int y = 0; y < speaker_.outChannel_; y++)
    {
        for (int x = 0; x < speaker_.inChannel_; x++)
        {
            outSpan[y] += (static_cast<float>(now[x]) /
                    static_cast<float>(std::numeric_limits<std::int16_t >::max())) *
                    outMatrix_[(y * speaker_.outChannel_) + x] *
                    speaker_.volume_;
        }
    }
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::GetPcm32NextFrame(
        std::span<float> outSpan,
       std::uint64_t size)
{
    std::span<std::int32_t> now{
            reinterpret_cast<std::int32_t *>(dataPtr_ + playBytye_),
            static_cast<std::size_t>(size /2ull )
    };
    for (int y = 0; y < speaker_.outChannel_; y++)
    {
        for (int x = 0; x < speaker_.inChannel_; x++)
        {
            outSpan[y] += (static_cast<float>(now[x]) /
                          static_cast<float>(std::numeric_limits<std::int32_t >::max())) *
                          outMatrix_[(y * speaker_.outChannel_) + x] * speaker_.volume_;
        }
    }
}
