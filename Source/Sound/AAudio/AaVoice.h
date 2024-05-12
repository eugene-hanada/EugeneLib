//
// Created by eugen on 2024/04/15.
//
#pragma once
#include <span>
#include <vector>
#include <cstdint>
#include <algorithm>

class AaSubmix;

namespace Eugene {
    class SoundBase;
}
class AaVoice
{
public:
    AaVoice(Eugene::SoundBase& soundBase,AaSubmix*  submix, std::uint32_t sample);
    virtual ~AaVoice();

    std::uint32_t GetSample() const
    {
        return sample_;
    }

    virtual void GetNextFrame(std::span<float> outSpan) {};
    void SetOutMatrix(std::span<float> volumes);
    void SetOut(AaSubmix* submix,std::uint16_t inChannel, std::uint16_t outChannel);
protected:
    void SetChannel(std::uint16_t inChannel, std::uint16_t outChannel);
    std::vector<float> outMatrix_;
    AaSubmix* submix_;
    std::uint32_t sample_;
    Eugene::SoundBase& soundBase_;
};

