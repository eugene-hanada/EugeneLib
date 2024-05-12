#pragma once
#include "../../../Include/Sound/SoundStreamSpeaker.h"
#include "AaSourceVoice.h"
#include <thread>
#include <atomic>
#include <semaphore>
#include <mutex>

class AaSubmix;

namespace Eugene
{
    class SoundStreamFile;

	class AaudioStreamSpeaker:
        public SoundStreamSpeaker
	{
	public:
        AaudioStreamSpeaker(std::unique_ptr<SoundStreamFile>&& streamFile,AaSubmix*  submix,std::uint16_t outChannel);
		~AaudioStreamSpeaker();

		void Play(int loopCount = 0) final;
		void Stop(void) final;
		bool IsEnd(void) const final;
		void SetPitchRate(float rate);
		void SetOutput(SoundControl& control) final;
		void SetVolume(float volume) final;
		void SetPan(std::span<float> volumes) final;
	private:

        struct StreamCallBack :
            public AaSourceVoice::CallBack
        {
            StreamCallBack(AaudioStreamSpeaker& streamSpeaker);
            void OnPlayEnd(std::int32_t loopCount) final;
            AaudioStreamSpeaker& streamSpeaker_;
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
	};
}