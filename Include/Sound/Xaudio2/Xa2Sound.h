#pragma once
#include "../../../Include/Sound/SoundBase.h"
#include "../../../Include/Utils/Utils.h"
#include <wrl.h>
#include <xaudio2.h>
#include "Xa2SoundSpeaker.h"
#include "Xa2SoundControl.h"
#include "Xa2SoundStreamSpeaker.h"

namespace Eugene
{
	class SoundStreamFile;

	/// <summary>
	/// サウンドクラス
	/// </summary>
	class Sound:
		public DynamicSingleton<Sound>, SoundBase
	{
	public:
		~Sound();
	
		void SetVolume(float volume) final;
	
		void SetPan(std::span<float> volumes) final;

		/// <summary>
		/// SoundSpeakerを生成
		/// </summary>
		/// <param name="soundFile"> サウンドファイル </param>
		/// <param name="maxPitchRate"> 最大ピッチ </param>
		/// <returns></returns>
		SoundSpeaker CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate = 2.0f)
		{
			return {xaudio2_.Get(), soundFile, inChannel_,maxPitchRate };
		}

		/// <summary>
		/// SoundControlを生成
		/// </summary>
		/// <param name="sample"> サンプル数 </param>
		/// <param name="stage"> 処理を行うステージ </param>
		/// <param name="inChannel"> 入力チャンネル </param>
		/// <returns></returns>
		SoundControl CreateSoundControl(std::uint32_t sample, std::uint32_t stage, std::uint16_t inChannel)
		{
			return { xaudio2_.Get(), sample, stage, inChannel, inChannel_ };
		}

		/// <summary>
		/// SoundStreamSpeakerを生成
		/// </summary>
		/// <param name="streamFile"> ストリーミング用サウンドファイル </param>
		/// <param name="maxPitchRate"> 最大ピッチ </param>
		/// <returns></returns>
		SoundStreamSpeaker CreateSoundStreamSpeaker(std::unique_ptr<SoundStreamFile>&& streamFile, const float maxPitchRate = 2.0f)
		{
			return { xaudio2_.Get(), std::move(streamFile), inChannel_, maxPitchRate };
		}

	private:
		Sound();
		
		/// <summary>
		/// IXAudio2MasteringVoiceをunique_ptrで使用するデリーター
		/// </summary>
		struct MasteringVoiceDeleter
		{
			void operator()(IXAudio2MasteringVoice* masteringVoice)
			{
				masteringVoice->DestroyVoice();
			}
		};

		/// <summary>
		/// チャンネルマスク
		/// </summary>
		std::uint32_t channelMask_;

		/// <summary>
		/// サンプリングレート
		/// </summary>
		std::uint32_t sampleRate_;

		/// <summary>
		/// IXAudio2
		/// </summary>
		Microsoft::WRL::ComPtr<IXAudio2> xaudio2_;

		/// <summary>
		///　マスタリングボイス
		/// </summary>
		std::unique_ptr<IXAudio2MasteringVoice, MasteringVoiceDeleter> mastering_;

		friend class DynamicSingleton<Sound>;
	};
}
