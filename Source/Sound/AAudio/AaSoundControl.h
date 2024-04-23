#pragma once
#include <memory>
#include <memory>

#include "../../../Include/Sound/SoundControl.h"
#include "AaVoice.h"
#include "AaSubmix.h"

namespace Eugene
{


	class SoundControl::SoundControlImpl :
        public AaSubmix
	{
	public:
		SoundControlImpl(std::uintptr_t devicePtr, SoundControl& control, std::uint32_t sample, std::uint32_t stage);
		~SoundControlImpl();
		void SetPan(std::span<float> volumes);
		void SetVolume(float volume);
		void SetOutput(SoundControl& control);
		void* Get(void);
		void SetOutChannel(std::uint16_t channel);
        void GetNextFrame(std::span<float> outSpan) final;
	private:
        SoundControl& control_;

	};
}


