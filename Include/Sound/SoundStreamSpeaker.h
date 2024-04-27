#pragma once
#include "SoundSpeaker.h"
#include <fstream>

namespace Eugene
{
	class SoundControl;

	/// <summary>
	/// ストリーミング再生するクラス
	/// </summary>
	class SoundStreamSpeaker :
		public SoundBase
	{
	public:
		
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="maxPitchRate"> 最大ピッチレート </param>
		SoundStreamSpeaker( std::uint16_t outChannel,const float maxPitchRate);
		~SoundStreamSpeaker();
		//void SetVolume(float volume);
		//void SetPan(std::span<float> volumes);

		//void SetOutput(SoundControl& control) final;

		virtual void Play(int loopCount = 0) = 0;
		virtual void Stop(void) = 0;
		virtual bool IsEnd(void) const = 0;
	 protected:


		 /// <summary>
		/// 最大ビッチレート
		/// </summary>
		const float maxPitchRate_;

		 /// <summary>
		 /// ファイル
		 /// </summary>
		 std::ifstream file_;
	};
}
