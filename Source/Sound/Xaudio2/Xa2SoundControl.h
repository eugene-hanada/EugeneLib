#pragma once
#include "../../../Include/Sound/SoundControl.h"
#include <xaudio2.h>

namespace Eugene
{

	class Xaudio2Control : 
		public SoundControl
	{
	public:
		Xaudio2Control(IXAudio2* device, std::uint32_t sample, std::uint32_t stage, std::uint16_t inChannel, std::uint16_t outChannel);
		~Xaudio2Control();
		void SetPan(std::span<float> volumes) final;
		void SetVolume(float volume) final;
		void SetOutput(SoundControl& control) final;
		void* Get(void) final;
		void SetOutChannel(std::uint16_t channel);
	private:
		struct SubmixVoiceDeleter
		{
			void operator()(IXAudio2SubmixVoice* submix)
			{
				submix->DestroyVoice();
			}
		};

		
		std::unique_ptr<IXAudio2SubmixVoice,SubmixVoiceDeleter> submix_;
	};
}
