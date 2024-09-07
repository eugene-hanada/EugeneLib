#pragma once
#include "../../../Include/Sound/SoundBase.h"
#include "../../../Include/Utils/Utils.h"
#include <wrl.h>
#include <xaudio2.h>
#include "Xa2SoundSpeaker.h"
#include "Xa2SoundControl.h"

namespace Eugene
{
	class Sound:
		public DynamicSingleton<Sound>, SoundBase
	{
	public:
		/*static void Create()
		{
			DynamicSingleton::Create();
		}*/


		~Sound();
	
		void SetVolume(float volume) final;
	
		void SetPan(std::span<float> volumes) final;

		SoundSpeaker CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate = 2.0f)
		{
			return {xaudio2_.Get(), soundFile, inChannel_,maxPitchRate };
		}

		SoundControl CreateSoundControl(std::uint32_t sample, std::uint32_t stage, std::uint16_t inChannel)
		{
			return { xaudio2_.Get(), sample, stage, inChannel, inChannel_ };
		}
		/*SoundSpeaker* CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate ) const final;

		SoundStreamSpeaker* CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const final;

		SoundControl* CreateSoundControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;

		Sound3DControl* CreateSound3DControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;*/
	private:
		Sound();
		


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

		Microsoft::WRL::ComPtr<IXAudio2> xaudio2_;
		std::unique_ptr<IXAudio2MasteringVoice, MasteringVoiceDeleter> mastering_;

		friend class DynamicSingleton<Sound>;
	};
}
