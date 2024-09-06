#pragma once
#ifdef EUGENE_XAUDIO
#include "Xaudio2/Xa2Sound.h"
#endif

namespace Eugene
{
	//class SoundSpeaker;
	//class SoundStreamSpeaker;
	//class SoundControl;
	//class Sound3DControl;
	//class SoundFile;


	//class Wave;
	//class OggVorbis;

	///// <summary>
	///// サウンド用のクラス
	///// </summary>
	//class Sound :
	//	public SoundBase
	//{
	//public:
	//	virtual ~Sound();

	//	/// <summary>
	//	/// サウンド再生するクラスを作成する
	//	/// </summary>
	//	/// <param name="wave"> サウンドファイルクラス </param>
	//	/// <returns></returns>
	//	[[nodiscard]]
	//	virtual SoundSpeaker* CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate = 2.0f) const = 0;


	//	/// <summary>
	//	/// ストリーミング再生(1秒づつ読み込み再生)するクラスを作成する(Waveファイルのみ対応)
	//	/// </summary>
	//	/// <param name="path"> ストリーミング再生を行うWaveファイルのパス </param>
	//	/// <returns></returns>
	//	[[nodiscard]]
	//	virtual SoundStreamSpeaker* CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate = 2.0f) const = 0;

	//	/// <summary>
	//	/// サウンドをコントロールするクラスを作成する
	//	/// </summary>
	//	/// <param name="sample"> サンプリング周波数 </param>
	//	/// <param name="inputChannel"> 入力チャンネル </param>
	//	/// <param name="outChannel"> 出力チャンネル </param>
	//	/// <returns></returns>
	//	[[nodiscard]]
	//	virtual SoundControl* CreateSoundControl(std::uint32_t stage = 0,std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const = 0;

	//	/// <summary>
	//	/// サウンドを3Dで出力しコントロールするクラスを作成する
	//	/// </summary>
	//	/// <param name="sample"> サンプリング周波数 </param>
	//	/// <param name="inputChannel"> 入力チャンネル </param>
	//	/// <param name="outChannel"> 出力チャンネル </param>
	//	/// <returns></returns>
	//	[[nodiscard]]
	//	virtual Sound3DControl* CreateSound3DControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const = 0;

	//	/// <summary>
	//	/// 全体のボリュームをセットする
	//	/// </summary>
	//	/// <param name="volume"> ボリューム </param>
	//	//void SetVolume(float volume) final;

	//	/// <summary>
	//	/// パンニング用にボリュームをセットする
	//	/// </summary>
	//	/// <param name="volumes"></param>
	//	//void SetPan(std::span<float> volumes) final;
	//protected:
	//	Sound();

	//	/// <summary>
	//	/// チャンネルマスク
	//	/// </summary>
	//	std::uint32_t channelMask_;

	//	/// <summary>
	//	/// サンプリングレート
	//	/// </summary>
	//	std::uint32_t sampleRate_;

	//	// コンストラクタアクセスのためにfriendにしてる
	//	friend Sound* CreateSound(void);
	//};

	///// <summary>
	///// サウンド用のクラスを生成する
	///// </summary>
	///// <param name=""></param>
	///// <returns></returns>
	//[[nodiscard]]
	//Sound* CreateSound(void);

	//using UniqueSound= std::unique_ptr<Sound>;

	///// <summary>
	///// CreateSoundのstd::_unique_ptr版
	///// </summary>
	///// <param name=""></param>
	///// <returns></returns>
	//UniqueSound CreateSoundUnique(void);
}
