#pragma once
#include "../../../Include/Sound/SoundStreamSpeaker.h"
#include <filesystem>
#include <thread>
#include <atomic>
#include <semaphore>
#include <memory>
#include <xaudio2.h>
#include <vector>



namespace Eugene
{
	
	class Xa2SoundStreamSpeaker :
		public SoundStreamSpeaker
	{
	public:
		Xa2SoundStreamSpeaker(IXAudio2* device,const std::filesystem::path& path, std::uint16_t outChannel);
		~Xa2SoundStreamSpeaker();
	private:
		class CollBack : public IXAudio2VoiceCallback
		{
		public:
			CollBack(Xa2SoundStreamSpeaker& speaker);
			void OnBufferEnd(void* pBufferContext) noexcept final;
			void OnBufferStart(void* pBufferContext) noexcept final;
			void OnLoopEnd(void* pBufferContext) noexcept final;
			void OnStreamEnd() noexcept final;
			void OnVoiceError(void* pBufferContext,HRESULT Error) noexcept final;
			void OnVoiceProcessingPassEnd() noexcept final;
			void OnVoiceProcessingPassStart(std::uint32_t BytesRequired) noexcept final;
		private:
			Xa2SoundStreamSpeaker& speaker_;
		};

		void Play(void) final;
		void Stop(void);
		bool IsEnd(void) const final;
		void SetPitchRate(float rate) final;
		void SetOutput(SoundControl& control) final;
		void SetVolume(float volume) final;
		void SetPan(std::span<float> volumes) final;

		void Worker(void);

		IXAudio2SourceVoice* source_;

		std::unique_ptr<CollBack> collback_;
		std::thread streamThread_;
		std::atomic_bool isRun_;
		std::atomic_bool isPlay_;
		std::binary_semaphore semaphore_{0};

		std::streampos starPos_;
		std::uint32_t dataSize_;
		std::uint32_t nowSize_;
		std::unique_ptr<XAUDIO2_BUFFER> buffer_;
		std::vector<std::uint8_t> bufferData_;
		std::vector<std::uint8_t> streamData_;
		std::uint32_t streamSize_;
		std::uint32_t bytesPerSec;

		friend class CollBack;
	};
}
