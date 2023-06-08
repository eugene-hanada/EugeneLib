#pragma once
#include "SoundBase.h"
#include <filesystem>

namespace Eugene
{
	class SoundSpeaker;
	class SoundStreamSpeaker;
	class SoundControl;
	class Sound3DControl;
	class SoundFile;


	class Wave;
	class OggVorbis;
	

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
		virtual SoundSpeaker* CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate = 2.0f) const = 0;

		/// <summary>
		/// サウンド再生をするクラスを作成する
		/// </summary>
		/// <param name=""></param>
		/// <param name="maxPitchRate"></param>
		/// <returns></returns>
		virtual SoundSpeaker* CreateSoundSpeaker(const OggVorbis&, const float maxPitchRate = 2.0f) const = 0;

		/// <summary>
		/// ストリーミング再生するクラスを作成する
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		virtual SoundStreamSpeaker* CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate = 2.0f) const = 0;

		/// <summary>
		/// サウンドをコントロールするクラスを作成する
		/// </summary>
		/// <param name="sample"></param>
		/// <param name="inputChannel"></param>
		/// <param name="outChannel"></param>
		/// <returns></returns>
		virtual SoundControl* CreateSoundControl(std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const = 0;

		/// <summary>
		/// サウンドを3Dで出力しコントロールするクラスを作成する
		/// </summary>
		/// <param name="sample"></param>
		/// <param name="inputChannel"></param>
		/// <param name="outChannel"></param>
		/// <returns></returns>
		virtual Sound3DControl* CreateSound3DControl(std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const = 0;
	protected:
		Sound();

		std::uint32_t channelMask_;

		std::uint32_t sampleRate_;
	};

	Sound* CreateSound(void);
}
