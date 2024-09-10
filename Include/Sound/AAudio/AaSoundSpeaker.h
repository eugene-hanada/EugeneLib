//
// Created by eugen on 2024/04/14.
//
#pragma once
#include "AaSourceVoice.h"
#include "../SoundBase.h"

class AaSubmix;

namespace Eugene {

    class SoundFile;
    class SoundControl;

    /// <summary>
    /// サウンドスピーカークラス
    /// </summary>
    class SoundSpeaker :
            public SoundBase
    {
    public:
        ~SoundSpeaker();

        void Play(int loopCount = 0);

        void Stop(void);

        bool IsEnd(void) const;

        void SetPitchRate(float rate);

        void SetVolume(float volume) final;

        void SetPan(std::span<float> volumes) final;

        void SetOutput(SoundControl &control) final;

        void SetData(const std::uint8_t *ptr, const std::uint64_t size);

        AaVoice* GetVoice() noexcept
        {
            return &voice_;
        }

        SoundSpeaker(SoundSpeaker&& speaker) noexcept :
                SoundBase{std::move(speaker)},voice_{std::move(speaker.voice_)}
        {
        }

        SoundSpeaker& operator=(SoundSpeaker&& speaker) noexcept
        {
            static_cast<SoundBase&>(*this) = std::move(speaker);
            voice_ = std::move(speaker.voice_);
            return *this;
        }

        void Final() noexcept
        {
            voice_.Stop();
        }

        SoundSpeaker(const SoundSpeaker&) = delete;
        SoundSpeaker& operator=(const SoundSpeaker&) = delete;
    private:
        SoundSpeaker(AaSubmix*  submix, const SoundFile &soundFile,std::uint16_t outChannel);
        void GetPcm16NextFrame(std::span<float> outSpan, std::uint64_t size);

        void GetPcm32NextFrame(std::span<float> outSpan, std::uint64_t size);

        AaSourceVoice voice_;
        friend class Sound;
    };

}