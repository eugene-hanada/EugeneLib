//
// Created by eugen on 2024/04/15.
//
#pragma once
#include <span>
#include <vector>
#include <cstdint>
#include <algorithm>

class AaSubmix;

class AaVoice
{
public:
    AaVoice(AaSubmix*  submix,std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t sample);
    virtual ~AaVoice();

    std::uint32_t GetSample() const
    {
        return sample_;
    }

    virtual void GetNextFrame(std::span<float> outSpan) {};

protected:
    void SetOut(AaSubmix* submix,std::uint16_t inChannel, std::uint16_t outChannel);
    void SetChannel(std::uint16_t inChannel, std::uint16_t outChannel);
    std::vector<float> outMatrix_;
    AaSubmix* submix_;
    std::uint32_t sample_;
};

