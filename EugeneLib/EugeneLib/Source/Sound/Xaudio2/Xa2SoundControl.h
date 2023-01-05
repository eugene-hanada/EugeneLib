#pragma once
#include "../../../Include/Sound/SoundControl.h"

struct IXAudio2;
struct IXAudio2SubmixVoice;

namespace EugeneLib
{

    class Xa2SoundControl :
        public SoundControl
    {
    public:
        Xa2SoundControl(IXAudio2* xaudio2, std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel);
    private:
        void* Get(void) final;
        void SetPan(std::span<float> volumes) final;
        void SetVolume(float volume) final;

        IXAudio2SubmixVoice* submix_;
    };
}
