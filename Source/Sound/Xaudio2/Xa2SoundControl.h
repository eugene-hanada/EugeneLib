#pragma once
#include "../../../Include/Sound/SoundControl.h"

struct IXAudio2;
struct IXAudio2SubmixVoice;

namespace Eugene
{

    class SoundControl::SoundControlImpl
    {
    public:
        SoundControlImpl(std::uintptr_t devicePtr, SoundControl& control,std::uint32_t sample, std::uint32_t stage);
        ~SoundControlImpl();
        void SetPan(std::span<float> volumes);
        void SetVolume(float volume) ;
        void SetOutput(SoundControl& control);
        void* Get(void);
        void SetOutChannel(std::uint16_t channel);
    private:
        SoundControl& control_;
        IXAudio2SubmixVoice* submix_;
    };
}
