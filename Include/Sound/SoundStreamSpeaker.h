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
		SoundStreamSpeaker(std::uintptr_t devicePtr, const std::filesystem::path& path, std::uint16_t outChannel,const float maxPitchRate);
		~SoundStreamSpeaker();
		void SetVolume(float volume);
		void SetPan(std::span<float> volumes);

		void SetOutput(SoundControl& control) final;
	 private:
		 class SoundStreamSpeakerImpl;

		 std::unique_ptr< SoundStreamSpeakerImpl> impl_;

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
