#pragma once
#include "SoundBase.h"
#include <memory>

namespace Eugene
{
	class Wave;
	class SoundControl;
	class SoundFile;

	/// <summary>
	/// サウンドを再生するクラス
	/// </summary>
	class SoundSpeaker :
		public SoundBase
	{
	public:

		~SoundSpeaker();

		/// <summary>
		/// 再生
		/// </summary>
		/// <param name=""></param>
		void Play(int loopCount = 0);

		/// <summary>
		/// 停止
		/// </summary>
		/// param name=""></param>
		void Stop(void);

		/// <summary>
		/// 再生が終了しているか?
		/// </summary>
		/// <param name=""></param>
		/// <returns> 終了時true、再生中の時false </returns>
		bool IsEnd(void) const;

		/// <summary>
		/// ピッチレートをセットする
		/// </summary>
		/// <param name="rate"></param>
		void SetPitchRate(float rate);

		/// <summary>
		/// 再生するサウンドデータをセットする
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="size"></param>
		void SetData(const std::uint8_t* ptr, const std::uint64_t size);


		void SetVolume(float volume) final;

		void SetPan(std::span<float> volumes) final;

		void SetOutput(SoundControl& control) final;
	private:

		class SoundSpeakerImpl;

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="maxPitchRate"> 最大ピッチレート </param>
		SoundSpeaker(std::uintptr_t devicePtr, const SoundFile& soundFile, std::uint16_t outChannel,const float maxPitchRate);

		/// <summary>
		/// 
		/// </summary>
		std::unique_ptr<SoundSpeakerImpl> impl_;

		/// <summary>
		/// 最大ビッチレート
		/// </summary>
		const float maxPitchRate_;


		// コンストラクタ公開のためfriend指定(生成時がSoundクラス依存なので致し方ない)
		friend class Sound;
	};
}
