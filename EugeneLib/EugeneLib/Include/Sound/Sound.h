#pragma once
#include "SoundBase.h"

namespace EugeneLib
{
	class SoundSpeaker;
	class SoundControl;
	class Wave;
	/// <summary>
	/// サウンド用のクラス
	/// </summary>
	class Sound :
		public SoundBase
	{
	public:
		virtual ~Sound();

		/// <summary>
		/// サウンド再生するクラスを作成する
		/// </summary>
		/// <param name="wave"></param>
		/// <returns></returns>
		virtual SoundSpeaker* CreateSoundSpeaker(const Wave& wave, const float maxPitchRate = 2.0f) const = 0;

		/// <summary>
		/// サウンドをコントロールするクラスを作成する
		/// </summary>
		/// <param name="sample"></param>
		/// <param name="inputChannel"></param>
		/// <param name="outChannel"></param>
		/// <returns></returns>
		virtual SoundControl* CreateSoundControl(std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel) const = 0;
	protected:
		Sound();
	};

	Sound* CreateSound(void);
}
