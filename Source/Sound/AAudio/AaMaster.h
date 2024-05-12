//
// Created by eugen on 2024/04/28.
//
#pragma once
#include "AaSubmix.h"
#include <span>
#include <cstdint>

class AaMaster :
        public AaSubmix
{
public:
    AaMaster(Eugene::SoundBase& soundBase,AaSubmix*  submix, std::uint32_t sample, std::uint64_t bufferSize);
    void Write(std::span<float> outAudio);
private:
    std::vector<float> buffer_;
};