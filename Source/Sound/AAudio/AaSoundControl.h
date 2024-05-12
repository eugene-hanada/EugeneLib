#pragma once
#include <memory>
#include <memory>
#include "../../../Include/Sound/SoundControl.h"
#include "AaVoice.h"
#include "AaSubmix.h"

namespace Eugene
{


	class AaudioControl:
		public SoundControl
	{
	public:
		AaudioControl(AaSubmix*  submix, std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t stage);
		~AaudioControl();
		void SetPan(std::span<float> volumes) final;
		void SetVolume(float volume) final;
		void SetOutput(SoundControl& control) final;
		void* Get(void) final;
		void SetOutChannel(std::uint16_t channel);
	private:
        AaSubmix submix_;

	};
}


