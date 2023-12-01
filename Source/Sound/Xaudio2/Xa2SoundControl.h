#pragma once
#include "../../../Include/Sound/SoundControl.h"
#include <xaudio2.h>

namespace Eugene
{

	class SoundControl::SoundControlImpl
	{
	public:
		SoundControlImpl(std::uintptr_t devicePtr, SoundControl& control,std::uint32_t sample, std::uint32_t stage);
		~SoundControlImpl();
		void SetPan(std::span<float> volumes);
		void SetVolume(float volume) ;
		void SetOutput(SoundControl& control);
		void* Get(void);
		void SetOutChannel(std::uint16_t channel);
	private:
		struct SubmixVoiceDeleter
		{
			void operator()(IXAudio2SubmixVoice* submix)
			{
				submix->DestroyVoice();
			}
		};

		SoundControl& control_;
		std::unique_ptr<IXAudio2SubmixVoice,SubmixVoiceDeleter> submix_;
	};
}
