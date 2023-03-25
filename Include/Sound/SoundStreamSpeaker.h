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
		public SoundSpeaker
	{
	public:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="maxPitchRate"> 最大ピッチレート </param>
		SoundStreamSpeaker(const float maxPitchRate);
		virtual ~SoundStreamSpeaker();
	 protected:

		 /// <summary>
		 /// ファイル
		 /// </summary>
		 std::ifstream file_;
	};
}
