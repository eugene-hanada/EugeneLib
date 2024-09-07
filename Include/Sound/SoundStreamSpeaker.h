#pragma once
#ifdef EUGENE_XAUDIO
#include "Xaudio2/Xa2SoundStreamSpeaker.h"
#endif

//#include "SoundSpeaker.h"
//#include <fstream>
//
//namespace Eugene
//{
//	class SoundControl;
//
//	/// <summary>
//	/// ストリーミング再生するクラス
//	/// </summary>
//	class SoundStreamSpeaker :
//		public SoundBase
//	{
//	public:
//		
//		/// <summary>
//		/// コンストラクタ
//		/// </summary>
//		/// <param name="maxPitchRate"> 最大ピッチレート </param>
//		SoundStreamSpeaker(std::uint16_t inChannel, std::uint16_t outChannel,const float maxPitchRate);
//		~SoundStreamSpeaker();
//
//		/// <summary>
//		/// 再生
//		/// </summary>
//		/// <param name="loopCount"> ループ回数 </param>
//		virtual void Play(int loopCount = 0) = 0;
//		
//		/// <summary>
//		/// 停止
//		/// </summary>
//		/// <param name=""></param>
//		virtual void Stop(void) = 0;
//		
//		/// <summary>
//		/// 再生終了したか？
//		/// </summary>
//		/// <param name=""></param>
//		/// <returns></return
//		/// s>
//		virtual bool IsEnd(void) const = 0;
//	 protected:
//
//
//		 /// <summary>
//		/// 最大ビッチレート
//		/// </summary>
//		const float maxPitchRate_;
//
//		 /// <summary>
//		 /// ファイル
//		 /// </summary>
//		 std::ifstream file_;
//	};
//}
