#pragma once
#include "../../../Include/Sound/Sound.h"
#include <wrl.h>
#include <xaudio2.h>

namespace Eugene
{
	class Xaudio2Sound:
		public Sound
	{
	public:
		Xaudio2Sound();
		~Xaudio2Sound();
	
		void SetVolume(float volume) final;
	
		void SetPan(std::span<float> volumes) final;

		SoundSpeaker* CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate ) const final;

		SoundStreamSpeaker* CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const final;

		SoundControl* CreateSoundControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;

		Sound3DControl* CreateSound3DControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;
	private:
		struct MasteringVoiceDeleter
		{
			void operator()(IXAudio2MasteringVoice* masteringVoice)
			{
				masteringVoice->DestroyVoice();
			}
		};
		Microsoft::WRL::ComPtr<IXAudio2> xaudio2_;
		std::unique_ptr<IXAudio2MasteringVoice, MasteringVoiceDeleter> mastering_;
	};
}
