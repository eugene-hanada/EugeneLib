#pragma once
#include "../../../Include/Sound/SoundStreamSpeaker.h"
#include <filesystem>
#include <thread>
#include <atomic>
#include <semaphore>
#include <mutex>
#include <memory>
#include <xaudio2.h>
#include <vector>



namespace Eugene
{
	class SoundStreamFile;

	class Xaudio2StreamSpeaker:
		public SoundStreamSpeaker
	{
	public:
		Xaudio2StreamSpeaker(IXAudio2* xaudio2,const std::filesystem::path& path, std::uint16_t outChannel, const float maxPitchRate);
		~Xaudio2StreamSpeaker();

		void Play(int loopCount = 0) final;
		void Stop(void) final;
		bool IsEnd(void) const final;
		void SetPitchRate(float rate);
		void SetOutput(SoundControl& control);
		void SetVolume(float volume);
		void SetPan(std::span<float> volumes);
	private:

		class CollBack : public IXAudio2VoiceCallback
		{
		public:
			CollBack(Xaudio2StreamSpeaker& speaker);
			void OnBufferEnd(void* pBufferContext) noexcept final;
			void OnBufferStart(void* pBufferContext) noexcept final;
			void OnLoopEnd(void* pBufferContext) noexcept final;
			void OnStreamEnd() noexcept final;
			void OnVoiceError(void* pBufferContext, HRESULT Error) noexcept final;
			void OnVoiceProcessingPassEnd() noexcept final;
			void OnVoiceProcessingPassStart(std::uint32_t BytesRequired) noexcept final;
		private:
			Xaudio2StreamSpeaker& speaker_;
		};

		struct SourceVoiceDeleter
		{
			void operator()(IXAudio2SourceVoice* voice)
			{
				voice->DestroyVoice();
			}
		};

		/// <summary>
		/// 再生ようにデータをセットアップする
		/// </summary>
		/// <param name=""></param>
		void SetUp(void);

		/// <summary>
		/// ストリーミング再生の作業を行う関数(別スレッドで動かす)
		/// </summary>
		/// <param name=""></param>
		void Worker(void);

		/// <summary>
		/// ソースボイス
		/// </summary>
		std::unique_ptr<IXAudio2SourceVoice, SourceVoiceDeleter> source_;

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

		std::unique_ptr<SoundStreamFile> streamFile_;

		/// <summary>
		/// ファイルの音声データ部分の開始位置
		/// </summary>
		//std::streampos starPos_;

		/// <summary>
		/// データサイズ
		/// </summary>
		//std::uint32_t dataSize_;

		/// <summary>
		/// 再生したサイズ
		/// </summary>
		//std::uint32_t nowSize_;

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
		std::uint64_t streamSize_;

		/// <summary>
		/// 1秒当たりのバイト数
		/// </summary>
		std::uint64_t bytesPerSec;

		/// <summary>
		/// 現在のループ数
		/// </summary>
		std::int32_t nowLoop_;

		/// <summary>
		/// ループの最大数
		/// </summary>
		std::int32_t maxLoop_;

		/// <summary>
		/// ミューテックス
		/// </summary>
		std::mutex mutex_;

		friend class CollBack;
	};
}
