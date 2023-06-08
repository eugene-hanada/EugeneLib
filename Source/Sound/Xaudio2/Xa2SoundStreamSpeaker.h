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
		Xa2SoundStreamSpeaker(IXAudio2* device,const std::filesystem::path& path, std::uint16_t outChannel, const float maxPitchRate);
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

		void Play(int loopCount = 0) final;
		void Stop(void);
		bool IsEnd(void) const final;
		void SetPitchRate(float rate) final;
		void SetOutput(SoundControl& control) final;
		void SetVolume(float volume) final;
		void SetPan(std::span<float> volumes) final;

		void Worker(void);

		/// <summary>
		/// ソースボイス
		/// </summary>
		IXAudio2SourceVoice* source_;

		/// <summary>
		/// コールバック用オブジェクト
		/// </summary>
		std::unique_ptr<CollBack> collback_;
		
		/// <summary>
		/// ストリーミングで使用するスレッド
		/// </summary>
		std::thread streamThread_;

		/// <summary>
		/// 実行フラグ
		/// </summary>
		std::atomic_bool isRun_;

		/// <summary>
		/// 再生フラグ
		/// </summary>
		std::atomic_bool isPlay_;

		/// <summary>
		/// セマフォ
		/// </summary>
		std::binary_semaphore semaphore_{0};

		/// <summary>
		/// 
		/// </summary>
		std::streampos starPos_;

		/// <summary>
		/// データサイズ
		/// </summary>
		std::uint32_t dataSize_;

		/// <summary>
		/// 再生したサイズ
		/// </summary>
		std::uint32_t nowSize_;

		/// <summary>
		/// バッファー
		/// </summary>
		std::unique_ptr<XAUDIO2_BUFFER> buffer_;

		/// <summary>
		/// バッファー用データ
		/// </summary>
		std::vector<std::uint8_t> bufferData_;

		/// <summary>
		/// ストリーミングで読み込んだデータ
		/// </summary>
		std::vector<std::uint8_t> streamData_;

		/// <summary>
		/// ストリーミングで読み込んだデータサイズ
		/// </summary>
		std::uint32_t streamSize_;

		/// <summary>
		/// 1秒当たりのバイト数
		/// </summary>
		std::uint32_t bytesPerSec;

		friend class CollBack;
	};
}
