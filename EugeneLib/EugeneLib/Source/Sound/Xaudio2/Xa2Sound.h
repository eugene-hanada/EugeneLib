#pragma once
#include "../../../Include/Sound/Sound.h"
#include <wrl.h>

struct IXAudio2;
struct IXAudio2MasteringVoice;

namespace EugeneLib
{
	class Xa2Sound :
		public Sound
	{
	public:
		Xa2Sound();
		~Xa2Sound();
	private:
		void SetVolume(float volume) final;

		SoundSpeaker* CreateSoundSpeaker(const Wave& wave) const final;

		Microsoft::WRL::ComPtr<IXAudio2> xaudio2_;
		IXAudio2MasteringVoice* mastering_;
	};
}
