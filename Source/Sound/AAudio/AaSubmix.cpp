#include "AaSubmix.h"
#include "resampler/MultiChannelResampler.h"
#include "../../../Include/Sound/SoundBase.h"

void AaSubmix::RemoveVoice(AaVoice *voice) {
    voices_.erase(voice);
}

void AaSubmix::AddVoice(AaVoice *voice) {
    auto voiceSample = voice->GetSample();
    if (voiceSample != sample_) {


        if (!resamplerMap_.contains(voiceSample)) {
            resamplerMap_.emplace(
                    voiceSample,
                    oboe::resampler::MultiChannelResampler::make(
                            soundBase_.GetInChannel(),
                            voice->GetSample(),
                            sample_,
                            oboe::resampler::MultiChannelResampler::Quality::Fastest
                    ));
        }
    }

    voices_.emplace(voice);
}

AaSubmix::AaSubmix(Eugene::SoundBase& soundBase,AaSubmix*  submix, std::uint32_t sample):
        AaVoice{soundBase,submix, sample}
{
    convertBuffer_.resize(soundBase_.GetInChannel());
}

AaSubmix::~AaSubmix() {

}

void AaSubmix::GetNextFrame(std::span<float> outSpan) {
    // フレーム単位で変換とミックスをする
    for (auto voice : voices_)
    {
        std::fill(convertBuffer_.begin(), convertBuffer_.end(), 0.0f);
        if (voice->GetSample() != AaSubmix::sample_)
        {
            auto outFrame = 1;
            auto& resampler = *resamplerMap_[voice->GetSample()];
            float* outBUfferPtr{outSpan.data()};

            while (outFrame > 0)
            {
                if (resampler.isWriteNeeded())
                {
                    voice->GetNextFrame(convertBuffer_);
                    resampler.writeNextFrame(convertBuffer_.data());
                }
                else
                {
                    resampler.readNextFrame(outBUfferPtr);
                    outFrame--;
                }
            }
        }
        else
        {
            voice->GetNextFrame(convertBuffer_);
        }

        for (std::uint16_t y = 0; y < soundBase_.GetOutChannel(); y++)
        {
            for (std::uint16_t x = 0; x < soundBase_.GetInChannel(); x++)
            {
                outSpan[y] += convertBuffer_[x] * outMatrix_[x + (y * soundBase_.GetOutChannel())] * soundBase_.GetVolume();
            }
        }
    }
}
