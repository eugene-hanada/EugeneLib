#include "AaSound.h"
#include "../../../Include/Utils//EugeneLibException.h"
#include "resampler/MultiChannelResampler.h"
#include "../../../Include/Sound/SoundFile.h"
#include "AaSoundSpeaker.h"
#include "AaSoundControl.h"
#include "AaSoundStreamSpeaker.h"
#include "../SoundStreamFile.h"

namespace {


    void OnErrorCallback(AAudioStream *stream,
                         void *userData,
                         aaudio_result_t error) {
        if (error == AAUDIO_ERROR_DISCONNECTED) {
            //
        }
    }
}

Eugene::AaudioSound::AaudioSound()  :
    Sound{}
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
        throw EugeneLibException{"AAudioストリームオープン失敗"};
    }

    aaudioStream_.reset(tmpStream);
    AAudioStreamBuilder_delete(streamBuilder);

    sampleRate_ = AAudioStream_getSampleRate(aaudioStream_.get());

    outChannel_ = inChannel_= AAudioStream_getChannelCount(aaudioStream_.get());
    AAudioStream_setBufferSizeInFrames(aaudioStream_.get(),AAudioStream_getFramesPerBurst(aaudioStream_.get()) * 2);

    master_ = std::make_unique<AaMaster>(*this,nullptr,sampleRate_,AAudioStream_getFramesPerBurst(aaudioStream_.get()) * 2 * inChannel_);
    if (AAudioStream_requestStart(aaudioStream_.get()) != AAUDIO_OK)
    {
        throw EugeneLibException{"AAudioスタート失敗"};
    }

}

Eugene::AaudioSound::~AaudioSound()
{

}

Eugene::Sound3DControl *
Eugene::AaudioSound::CreateSound3DControl(
        std::uint32_t stage, std::uint32_t sample,
        std::uint16_t inputChannel,
        std::uint16_t outChannel) const
{
    return nullptr;
}

Eugene::SoundControl *
Eugene::AaudioSound::CreateSoundControl(
    std::uint32_t stage, std::uint32_t sample,
    std::uint16_t inputChannel,
    std::uint16_t outChannel) const
{
    return new AaudioControl{master_.get(),sample,inputChannel,outChannel,stage};
}

Eugene::SoundSpeaker *Eugene::AaudioSound::CreateSoundSpeaker(
        const Eugene::SoundFile &soundFile,
        const float maxPitchRate) const
{
    return new AaudioSpeaker{master_.get(),soundFile,inChannel_};
}

Eugene::SoundStreamSpeaker* Eugene::AaudioSound::CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const
{
    try {
        return new AaudioStreamSpeaker{CreateSoundStreamFile(path),master_.get(), inChannel_};
    }
    catch (const std::exception& e)
    {
        return nullptr;
    }
}


void Eugene::AaudioSound::SetVolume(float volume)
{
    volume_ = volume * volume;
}


void Eugene::AaudioSound::SetPan(std::span<float> volumes)
{
    master_->SetOutMatrix(volumes);
}

aaudio_data_callback_result_t Eugene::AaudioSound::OnDataCallBack(
        AAudioStream *stream,
        void *userData,
        void *audioData,
        std::int32_t numFrames)
{
    reinterpret_cast<AaudioSound*>(userData)->Write(audioData,numFrames);
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void Eugene::AaudioSound::Write(void* audioData, std::int32_t numFrames)
{
    std::span<float> outData{static_cast<float*>(audioData),static_cast<std::size_t>(numFrames * inChannel_)};
    std::fill(outData.begin(), outData.end(),0.0f);
    master_->Write(outData);
}