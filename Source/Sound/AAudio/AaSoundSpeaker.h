//
// Created by eugen on 2024/04/14.
//
#pragma once
#include "../../../Include/Sound/SoundSpeaker.h"
#include "AaSourceVoice.h"

class AaSubmix;

namespace Eugene {

    class SoundFile;
    class AaudioSpeaker : 
        public SoundSpeaker
    {
    public:
        AaudioSpeaker(AaSubmix*  submix, const SoundFile &soundFile,std::uint16_t outChannel);

        ~AaudioSpeaker();

        void Play(int loopCount = 0) final;

        void Stop(void) final;

        bool IsEnd(void) const final;

        void SetPitchRate(float rate) final;

        void SetVolume(float volume) final;

        void SetPan(std::span<float> volumes) final;

        void SetOutput(SoundControl &control) final;

        void SetData(const std::uint8_t *ptr, const std::uint64_t size) final;

        AaVoice* GetVoice();
    private:
        void GetPcm16NextFrame(std::span<float> outSpan, std::uint64_t size);

        void GetPcm32NextFrame(std::span<float> outSpan, std::uint64_t size);

        AaSourceVoice voice_;
    };

}