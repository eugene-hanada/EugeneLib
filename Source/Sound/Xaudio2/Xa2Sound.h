#pragma once
#include "../../../Include/Sound/Sound.h"
#include <wrl.h>

struct IXAudio2;
struct IXAudio2MasteringVoice;

namespace Eugene
{
	class Sound::SoundImpl
	{
	public:
		SoundImpl(Sound& sound);
		~SoundImpl();
	
		void SetVolume(float volume);
	
		void SetPan(std::span<float> volumes);

		SoundSpeaker* CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate = 2.0f) const;

		SoundStreamSpeaker* CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate = 2.0f) const ;

		SoundControl* CreateSoundControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const ;

		Sound3DControl* CreateSound3DControl(std::uint32_t stage = 0, std::uint32_t sample = 0u, std::uint16_t inputChannel = 0u, std::uint16_t outChannel = 0u) const ;
	private:
		Sound& sound_;
		Microsoft::WRL::ComPtr<IXAudio2> xaudio2_;
		IXAudio2MasteringVoice* mastering_;
	};
}
