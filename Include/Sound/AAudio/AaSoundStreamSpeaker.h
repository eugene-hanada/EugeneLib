#pragma once
#include "AaSourceVoice.h"
#include <thread>
#include <atomic>
#include <semaphore>
#include <mutex>
#include "../SoundBase.h"

class AaSubmix;

namespace Eugene
{
    class SoundStreamFile;

	class SoundStreamSpeaker :
            public SoundBase
	{
	public:
		~SoundStreamSpeaker();

		void Play(int loopCount = 0);
		void Stop(void);
		bool IsEnd(void) const ;
		void SetPitchRate(float rate);
		void SetOutput(SoundControl& control) final;
		void SetVolume(float volume) final;
		void SetPan(std::span<float> volumes) final;

        SoundStreamSpeaker(SoundStreamSpeaker&& streamSpeaker) noexcept ;
        SoundStreamSpeaker& operator=(SoundStreamSpeaker&& streamSpeaker) noexcept;
        SoundStreamSpeaker(const SoundStreamSpeaker&) = delete;
        SoundStreamSpeaker& operator=(const SoundStreamSpeaker&) = delete;
	private:
        SoundStreamSpeaker(std::unique_ptr<SoundStreamFile>&& streamFile,AaSubmix*  submix,std::uint16_t outChannel);
        struct StreamCallBack :
            public AaSourceVoice::CallBack
        {
            StreamCallBack(SoundStreamSpeaker& streamSpeaker);
            void OnPlayEnd(std::int32_t loopCount) final;
            SoundStreamSpeaker* streamSpeaker_;
            StreamCallBack(StreamCallBack&& callBack) noexcept:
                streamSpeaker_{callBack.streamSpeaker_}
            {
                callBack.streamSpeaker_ = nullptr;
            }

            StreamCallBack& operator=(StreamCallBack&& callBack)
            {
                streamSpeaker_ = callBack.streamSpeaker_;
                callBack.streamSpeaker_ = nullptr;
                return *this;
            }

            void Set( SoundStreamSpeaker* streamSpeaker) noexcept
            {
                streamSpeaker_ = streamSpeaker;
            }
        };

        friend class StreamCallBack;

        void SetUp(void);

        void Worker(void);

        AaSourceVoice voice_;
        StreamCallBack callBack_;
        std::unique_ptr<SoundStreamFile> streamFile_;
        std::vector<uint8_t> streamData_;
        std::vector<uint8_t> bufferData_;


        std::thread streamThread_;
        std::atomic_bool isRun_;
        std::atomic_bool isPlay_;
        std::binary_semaphore semaphore_{0};
        std::uint64_t streamSize_;
        std::int32_t nowLoop_;
        std::int32_t maxLoop_;
        std::mutex mutex_;

        friend class Sound;
	};
}