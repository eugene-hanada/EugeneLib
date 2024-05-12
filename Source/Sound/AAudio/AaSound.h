#pragma once
#include "../../../Include/Sound/Sound.h"
#include <memory>
#include <aaudio/AAudio.h>
#include <vector>
#include "AaMaster.h"


namespace Eugene
{
    class AaudioSound:
            public Sound
    {
    public:
        AaudioSound();

        ~AaudioSound();

        void SetVolume(float volume) final;

        void SetPan(std::span<float> volumes) final;

        SoundSpeaker* CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate ) const final;

        SoundStreamSpeaker* CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const final;

        SoundControl* CreateSoundControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;

        Sound3DControl* CreateSound3DControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;
    private:

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


        std::unique_ptr<AAudioStream, AAudioStreamDeleter> aaudioStream_;
    };
}
