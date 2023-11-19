#pragma once
#include "../../../Include/Sound/Sound.h"
#include <wrl.h>

struct IXAudio2;
struct IXAudio2MasteringVoice;

namespace Eugene
{
	class Xa2Sound :
		public Sound
	{
	public:
		Xa2Sound();
		~Xa2Sound();
	private:
		void SetVolume(float volume) final;
	
		void SetPan(std::span<float> volumes) final;

		SoundSpeaker* CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate = 2.0f) const final;

		SoundStreamSpeaker* CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate = 2.0f) const final;

		SoundControl* CreateSoundControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;

		Sound3DControl* CreateSound3DControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const final;

		Microsoft::WRL::ComPtr<IXAudio2> xaudio2_;
		IXAudio2MasteringVoice* mastering_;
	};
}
