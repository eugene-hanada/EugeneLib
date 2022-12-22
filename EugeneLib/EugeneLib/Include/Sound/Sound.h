#pragma once

namespace EugeneLib
{
	class SoundSpeaker;
	class Wave;
	/// <summary>
	/// サウンド用のクラス
	/// </summary>
	class Sound
	{
	public:
		virtual ~Sound();

		/// <summary>
		/// サウンド再生するクラスを作成する
		/// </summary>
		/// <param name="wave"></param>
		/// <returns></returns>
		virtual SoundSpeaker* CreateSoundSpeaker(const Wave& wave) const = 0;

		/// <summary>
		/// 全体のボリュームをセットする
		/// </summary>
		/// <param name="volume"> ボリューム </param>
		virtual void SetVolume(float volume) = 0;

	protected:
		Sound();
	};

	Sound* CreateSound(void);
}
