#include "AaSound.h"
#include "../../../Include/Sound/SoundSpeaker.h"
#include "../../../Include/Sound/SoundControl.h"
#include "AaVoice.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "resampler/MultiChannelResampler.h"
#include "../../../Include/Sound/SoundFile.h"

namespace {


    void OnErrorCallback(AAudioStream *stream,
                         void *userData,
                         aaudio_result_t error) {
        if (error == AAUDIO_ERROR_DISCONNECTED) {
            //
        }
    }
}

Eugene::Sound::SoundImpl::SoundImpl(Eugene::Sound &sound)  :
    sound_{sound}, AaSubmix{nullptr,48000,2,2}
{

    AAudioStreamBuilder* streamBuilder{nullptr};
    AAudio_createStreamBuilder(&streamBuilder);
    AAudioStreamBuilder_setPerformanceMode(
            streamBuilder,
            AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setDataCallback(
            streamBuilder,
            OnDataCallBack,
            this);
    AAudioStreamBuilder_setErrorCallback(
            streamBuilder,
            OnErrorCallback,
            this);

    AAudioStreamBuilder_setFormat(streamBuilder,AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStream * tmpStream{nullptr};
    if (AAudioStreamBuilder_openStream(streamBuilder, &tmpStream) != AAUDIO_OK)
    {
        throw CreateErrorException{"AAudioストリームオープン失敗"};
    }

    aaudioStream_.reset(tmpStream);
    AAudioStreamBuilder_delete(streamBuilder);

    sound_.sampleRate_ = AAudioStream_getSampleRate(aaudioStream_.get());
    sample_ = sound_.sampleRate_;
    channel_ = sound_.outChannel_ = sound_.inChannel_= AAudioStream_getChannelCount(aaudioStream_.get());
    AAudioStream_setBufferSizeInFrames(aaudioStream_.get(),AAudioStream_getFramesPerBurst(aaudioStream_.get()) * 2);
    outMatrix_.resize(sound_.outChannel_ * sound_.inChannel_);

    std::fill(outMatrix_.begin(),outMatrix_.end(), 0.0f);
    for (std::uint16_t y = 0; y < sound_.outChannel_; y++)
    {
        for (std::uint16_t x = 0; x < sound_.inChannel_; x++)
        {
            if (x == y)
            {
                outMatrix_[x + (y * sound_.outChannel_)] = 1.0f;
            }
        }
    }

    convertBuffer_.resize(sound_.inChannel_);
    buffer_.resize(AAudioStream_getFramesPerBurst(aaudioStream_.get())  * sound_.inChannel_);

    if (AAudioStream_requestStart(aaudioStream_.get()) != AAUDIO_OK)
    {
        throw CreateErrorException{"AAudioスタート失敗"};
    }

}

Eugene::Sound::SoundImpl::~SoundImpl()
{

}

Eugene::Sound3DControl *
Eugene::Sound::SoundImpl::CreateSound3DControl(
        std::uint32_t stage, std::uint32_t sample,
        std::uint16_t inputChannel,
        std::uint16_t outChannel) const
{
    return nullptr;
}

Eugene::SoundControl *
Eugene::Sound::SoundImpl::CreateSoundControl(
    std::uint32_t stage, std::uint32_t sample,
    std::uint16_t inputChannel,
    std::uint16_t outChannel) const
{
    return new SoundControl{
            reinterpret_cast<std::uintptr_t>(this),
            sample,
            inputChannel,
            outChannel,
            0
    };
}

Eugene::SoundSpeaker *Eugene::Sound::SoundImpl::CreateSoundSpeaker(
        const Eugene::SoundFile &soundFile,
        const float maxPitchRate) const
{
    return new SoundSpeaker{reinterpret_cast<std::uintptr_t>(this),soundFile,sound_.inChannel_,1.0f};
}

Eugene::SoundStreamSpeaker* Eugene::Sound::SoundImpl::CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const
{
    return nullptr;
}


void Eugene::Sound::SoundImpl::SetVolume(float volume)
{
    sound_.volume_ = volume * volume;
}


void Eugene::Sound::SoundImpl::SetPan(std::span<float> volumes)
{
    std::copy(volumes.begin(),volumes.end(),outMatrix_.begin());
}

aaudio_data_callback_result_t Eugene::Sound::SoundImpl::OnDataCallBack(
        AAudioStream *stream,
        void *userData,
        void *audioData,
        std::int32_t numFrames)
{
    reinterpret_cast<SoundImpl*>(userData)->Write(audioData,numFrames);
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void Eugene::Sound::SoundImpl::Write(void* audioData, std::int32_t numFrames)
{
    std::span<float> outData{static_cast<float*>(audioData),static_cast<std::size_t>(numFrames * sound_.inChannel_)};
    std::fill(outData.begin(), outData.end(),0.0f);
    for (auto voice : voices_)
    {
        std::fill(buffer_.begin(), buffer_.end(),0.0f);
        if (voice->GetSample() != sound_.sampleRate_)
        {
            auto outFrame = numFrames;
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
                    outBUfferPtr += sound_.inChannel_;
                    outFrame--;
                }
            }
        }
        else
        {
            voice->GetNextFrame({buffer_.data(),outData.size()});
        }


        for (std::uint64_t i = 0; i < numFrames * sound_.inChannel_; i++)
        {
            for (std::uint16_t y = 0; y < sound_.outChannel_; y++)
            {
                for (std::uint16_t x = 0; x < sound_.inChannel_; x++)
                {
                    outData[i] += buffer_[i] * outMatrix_[x + (y * sound_.outChannel_)] * sound_.volume_ ;
                }
            }
        }

    }
}