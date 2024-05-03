#pragma once
#include <memory>
#include <set>
#include <map>
#include <span>
#include <vector>
#include "resampler/MultiChannelResampler.h"
#include "AaVoice.h"

class AaSubmix :
        public AaVoice
{
public:
    AaSubmix(Eugene::SoundBase& soundBase,AaSubmix*  submix, std::uint32_t sample);
    virtual ~AaSubmix();
    void AddVoice(AaVoice* voice);
    void RemoveVoice(AaVoice* voice);
    void GetNextFrame(std::span<float> outSpan) final;
protected:
    std::set<AaVoice*> voices_;
    std::map<
    std::uint32_t,
    std::unique_ptr<oboe::resampler::MultiChannelResampler>
    > resamplerMap_;
    std::vector<float> convertBuffer_;

};
