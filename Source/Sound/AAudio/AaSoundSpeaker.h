//
// Created by eugen on 2024/04/14.
//
#pragma once
#include "../../../Include/Sound/SoundSpeaker.h"
#include "AaVoice.h"


namespace Eugene {

    class SoundFile;
    class SoundSpeaker::SoundSpeakerImpl :
            public AaVoice
    {
    public:
        SoundSpeakerImpl(std::uintptr_t device, SoundSpeaker &speaker, const SoundFile &soundFile);

        ~SoundSpeakerImpl();

        void Play(int loopCount = 0);

        void Stop(void);

        bool IsEnd(void) const;

        void SetPitchRate(float rate);

        void SetVolume(float volume);

        void SetPan(std::span<float> volumes);

        void SetOutput(SoundControl &control);

        void SetData(const std::uint8_t *ptr, const std::uint64_t size);

    private:

        void GetNextFrame(std::span<float> outSpan) final;

        void GetPcm16NextFrame(std::span<float> outSpan, std::uint64_t size);

        void GetPcm32NextFrame(std::span<float> outSpan, std::uint64_t size);

        /// <summary>
        ///
        /// </summary>
        SoundSpeaker &speaker_;

        std::uint8_t* dataPtr_;
        std::uint64_t  dataSize_;
        std::int32_t loopCount_;
        std::uint64_t playBytye_;
        std::uint64_t byteParFrame_;

        void (SoundSpeakerImpl::* getNextFrameFunc_)(std::span<float> , std::uint64_t);

    };

}