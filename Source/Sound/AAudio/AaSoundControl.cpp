#include "AaSoundControl.h"
#include "resampler/MultiChannelResampler.h"
#include "../../../Include/Common/EugeneLibException.h"

Eugene::SoundControl::SoundControlImpl::SoundControlImpl(std::uintptr_t devicePtr, SoundControl& control, std::uint32_t sample, std::uint32_t stage) :
        AaSubmix{reinterpret_cast<AaSubmix*>(devicePtr),control.inChannel_,control.outChannel_,sample},
        control_{ control }
{

}

Eugene::SoundControl::SoundControlImpl::~SoundControlImpl()
{
}

void* Eugene::SoundControl::SoundControlImpl::Get(void)
{
	return this;
}

void Eugene::SoundControl::SoundControlImpl::SetOutChannel(std::uint16_t channel)
{
	control_.outChannel_ = channel;
}

void Eugene::SoundControl::SoundControlImpl::SetPan(std::span<float> volumes)
{
	std::copy(volumes.begin(),volumes.end(),outMatrix_.begin());
}

void Eugene::SoundControl::SoundControlImpl::SetVolume(float volume)
{
	control_.volume_ = volume * volume;
}

void Eugene::SoundControl::SoundControlImpl::SetOutput(SoundControl& control)
{
    control_.outChannel_ = control.inChannel_;
    auto submix = static_cast<AaSubmix*>(control.Get());
    SetOut(submix, control_.inChannel_,control_.outChannel_);

}

void Eugene::SoundControl::SoundControlImpl::GetNextFrame(std::span<float> outSpan)
{
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

        for (std::uint16_t y = 0; y < control_.outChannel_; y++)
        {
            for (std::uint16_t x = 0; x < control_.inChannel_; x++)
            {
                outSpan[y] += convertBuffer_[x] * outMatrix_[x + (y * control_.outChannel_)] * control_.volume_;
            }
        }
    }

}
