#pragma once
#include "../../../Include/Sound/SoundControl.h"

struct IXAudio2;
struct IXAudio2SubmixVoice;

namespace Eugene
{

    class Xa2SoundControl :
        public SoundControl
    {
    public:
        Xa2SoundControl(IXAudio2* xaudio2, std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel, std::uint32_t stage);
        ~Xa2SoundControl();
        void SetPan(std::span<float> volumes) final;
        void SetVolume(float volume) final;
        void SetOutput(SoundControl& control) final;
    private:
        void* Get(void) final;
        IXAudio2SubmixVoice* submix_;
    };
}
