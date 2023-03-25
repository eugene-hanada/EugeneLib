#pragma once
#include "SoundBase.h"

namespace Eugene
{
	class Wave;
	class SoundControl;

	/// <summary>
	/// サウンドを再生するクラス
	/// </summary>
	class SoundSpeaker :
		public SoundBase
	{
	public:

		virtual ~SoundSpeaker();

		/// <summary>
		/// 再生
		/// </summary>
		/// <param name=""></param>
		virtual void Play(void) = 0;

		/// <summary>
		/// 停止
		/// </summary>
		/// <param name=""></param>
		virtual void Stop(void) = 0;

		/// <summary>
		/// 再生が終了しているか?
		/// </summary>
		/// <param name=""></param>
		/// <returns> 終了時true、再生中の時false </returns>
		virtual bool IsEnd(void) const = 0;

		/// <summary>
		/// ピッチレートをセットする
		/// </summary>
		/// <param name="rate"></param>
		virtual void SetPitchRate(float rate) = 0;

		/// <summary>
		/// サウンドコントロールクラスに出力する(出力先でコントロールできる)
		/// </summary>
		/// <param name="control"> サウンドコントロールクラス </param>
		virtual void SetOutput(SoundControl& control) = 0;

	protected:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="maxPitchRate"> 最大ピッチレート </param>
		SoundSpeaker(const float maxPitchRate);
		
		/// <summary>
		/// 最大ビッチレート
		/// </summary>
		const float maxPitchRate_;
	};
}
