#include "../../../Include/Sound/AAudio/AaSound.h"
#include "../../../Include/Utils//EugeneLibException.h"
#include "../../../Include/Sound/AAudio/resampler/MultiChannelResampler.h"
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

Eugene::Sound::Sound()
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

    master_ = std::make_unique<AaMaster>(static_cast<SoundBase&>(*this),nullptr,sampleRate_,AAudioStream_getFramesPerBurst(aaudioStream_.get()) * 2 * inChannel_);
    if (AAudioStream_requestStart(aaudioStream_.get()) != AAUDIO_OK)
    {
        throw EugeneLibException{"AAudioスタート失敗"};
    }

}

Eugene::Sound::~Sound()
{

}

void Eugene::Sound::SetVolume(float volume)
{
    volume_ = volume * volume;
}


void Eugene::Sound::SetPan(std::span<float> volumes)
{
    master_->SetOutMatrix(volumes);
}

aaudio_data_callback_result_t Eugene::Sound::OnDataCallBack(
        AAudioStream *stream,
        void *userData,
        void *audioData,
        std::int32_t numFrames)
{
    reinterpret_cast<Sound*>(userData)->Write(audioData,numFrames);
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void Eugene::Sound::Write(void* audioData, std::int32_t numFrames)
{
    std::span<float> outData{static_cast<float*>(audioData),static_cast<std::size_t>(numFrames * inChannel_)};
    std::fill(outData.begin(), outData.end(),0.0f);
    master_->Write(outData);
}