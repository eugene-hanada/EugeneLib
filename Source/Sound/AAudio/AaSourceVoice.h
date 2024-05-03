//
// Created by eugen on 2024/04/27.
//
#pragma once
#include "AaVoice.h"
#include "../../../Include/Sound/SoundCommon.h"

class AaSourceVoice :
        public AaVoice
{
public:
    AaSourceVoice(Eugene::SoundBase& soundBase,AaSubmix*  submix, const Eugene::SoundFormat& format);
    void SetData(const std::uint8_t* ptr, const std::uint64_t size);
    void Play(std::int32_t loopCount);
    void Stop();
    bool IsEnd() const;
private:
    void GetNextFrame(std::span<float> outSpan) final;
    void GetPcm16NextFrame(std::span<float> outSpan, std::uint64_t size);
    void GetPcm32NextFrame(std::span<float> outSpan, std::uint64_t size);

    std::uint8_t* dataPtr_;
    std::uint64_t  dataSize_;
    std::int32_t loopCount_;
    std::uint64_t playBytye_;
    std::uint64_t byteParFrame_;
    void (AaSourceVoice::* getNextFrameFunc_)(std::span<float> , std::uint64_t);
};

