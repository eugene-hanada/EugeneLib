#pragma once
#include "../../../Include/Sound/Sound.h"
#include <memory>
#include <aaudio/AAudio.h>
#include <vector>
#include "AaSubmix.h"

class AaVoice;

namespace Eugene
{
    class Sound::SoundImpl :
            public AaSubmix
    {
    public:
        SoundImpl(Sound& sound);

        ~SoundImpl();

        void SetVolume(float volume);

        void SetPan(std::span<float> volumes);

        SoundSpeaker* CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate ) const;

        SoundStreamSpeaker* CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const ;

        SoundControl* CreateSoundControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const ;

        Sound3DControl* CreateSound3DControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const ;
    private:

        static aaudio_data_callback_result_t OnDataCallBack(
                AAudioStream *stream,
                void *userData,
                void *audioData,
                std::int32_t numFrames);

        void Write(void* audioData, std::int32_t numFrames);

        Sound& sound_;

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


        std::vector<float> buffer_;
    };
}
