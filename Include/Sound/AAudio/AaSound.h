#pragma once
#include <memory>
#include <aaudio/AAudio.h>
#include <vector>
#include "AaMaster.h"
#include "../SoundBase.h"
#include "../../Utils/Utils.h"
#include "AaSoundSpeaker.h"
#include "AaSoundControl.h"
#include "AaSoundStreamSpeaker.h"

namespace Eugene
{
    class Sound :
            public DynamicSingleton<Sound> ,SoundBase
    {
    public:

        ~Sound();

        SoundSpeaker CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate = 2.0f)
        {
            return {master_.get(), soundFile, inChannel_};
        }

        SoundControl CreateSoundControl(std::uint32_t sample, std::uint32_t stage, std::uint16_t inChannel)
        {
            return {master_.get(), sample, inChannel,inChannel_, stage};
        }

        SoundStreamSpeaker CreateSoundStreamSpeaker(std::unique_ptr<SoundStreamFile>&& streamFile, const float maxPitchRate = 2.0f)
        {
            return {std::move(streamFile), master_.get(), inChannel_};
        }

        void SetVolume(float volume) final;

        void SetPan(std::span<float> volumes) final;

        //SoundSpeaker* CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate ) const final;

        //SoundStreamSpeaker* CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const final;

        //SoundControl* CreateSoundControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;

        //Sound3DControl* CreateSound3DControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;
    private:
        Sound();
        static aaudio_data_callback_result_t OnDataCallBack(
                AAudioStream *stream,
                void *userData,
                void *audioData,
                std::int32_t numFrames);

        void Write(void* audioData, std::int32_t numFrames);

        std::unique_ptr<AaMaster> master_;

        struct AAudioStreamDeleter
        {
            void operator()(AAudioStream* ptr)
            {
                AAudioStream_requestStop(ptr);
                AAudioStream_close(ptr);
                AAudioStream_release(ptr);
            }
        };

        float sampleRate_;
        std::unique_ptr<AAudioStream, AAudioStreamDeleter> aaudioStream_;

        friend class DynamicSingleton<Sound>;
    };
}
