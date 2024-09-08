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

    AaSubmix(AaSubmix&& submix) noexcept :
            AaVoice{std::move(submix)},voices_{std::move(submix.voices_)}, resamplerMap_{std::move(submix.resamplerMap_)},
            convertBuffer_{std::move(submix.convertBuffer_)}
    {
    }

    AaSubmix& operator=(AaSubmix&& submix) noexcept
    {
        static_cast<AaVoice&>(*this) = std::move(submix);
        voices_ = std::move(submix.voices_);
        resamplerMap_ = std::move(submix.resamplerMap_);
        convertBuffer_ = std::move(submix.convertBuffer_);
    }

    AaSubmix(const AaSubmix&) = delete;
    AaSubmix& operator=(const AaSubmix&) = delete;
protected:
    std::set<AaVoice*> voices_;
    std::map<
    std::uint32_t,
    std::unique_ptr<oboe::resampler::MultiChannelResampler>
    > resamplerMap_;
    std::vector<float> convertBuffer_;

};
