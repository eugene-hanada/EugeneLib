#include "AaSubmix.h"
#include "resampler/MultiChannelResampler.h"


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
                            channel_,
                            voice->GetSample(),
                            sample_,
                            oboe::resampler::MultiChannelResampler::Quality::Fastest
                    ));
        }
    }

    voices_.emplace(voice);
}

AaSubmix::AaSubmix(AaSubmix*  submix,std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t sample):
        AaVoice{submix,inChannel,outChannel, sample},channel_{inChannel}
{
    convertBuffer_.resize(channel_);
}

AaSubmix::~AaSubmix() {

}
