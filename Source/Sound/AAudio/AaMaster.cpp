//
// Created by eugen on 2024/04/28.
//

#include "AaMaster.h"
#include "../../../Include/Sound/SoundBase.h"

void AaMaster::Write(std::span<float> outAudio)
{
    for (auto voice : voices_)
    {
        std::fill(buffer_.begin(), buffer_.end(),0.0f);
        if (voice->GetSample() != sample_)
        {
            auto outFrame = outAudio.size() / soundBase_.GetInChannel();
            auto& resampler = *resamplerMap_[voice->GetSample()];
            float* outBUfferPtr{buffer_.data()};
            if (buffer_.size() < outFrame)
            {
                return;
            }
            while (outFrame > 0)
            {
                if (resampler.isWriteNeeded())
                {
                    voice->GetNextFrame(convertBuffer_);
                    resampler.writeNextFrame(convertBuffer_.data());
                    std::fill(convertBuffer_.begin(), convertBuffer_.end(),0.0f);
                }
                else
                {
                    resampler.readNextFrame(outBUfferPtr);
                    outBUfferPtr += soundBase_.GetInChannel();
                    outFrame--;
                }
            }
        }
        else
        {
            voice->GetNextFrame({buffer_.data(),outAudio.size()});
        }


        for (std::uint64_t i = 0; i < outAudio.size(); i++)
        {
            for (std::uint16_t y = 0; y < soundBase_.GetOutChannel(); y++)
            {
                for (std::uint16_t x = 0; x < soundBase_.GetInChannel(); x++)
                {
                    outAudio[i] += buffer_[i] * outMatrix_[x + (y * soundBase_.GetOutChannel())] * soundBase_.GetVolume();
                }
            }
        }
    }
}

AaMaster::AaMaster(Eugene::SoundBase& soundBase,AaSubmix*  submix, std::uint32_t sample, std::uint64_t bufferSize) :
        AaSubmix{soundBase, submix, sample}
{
    buffer_.resize(bufferSize);
}
