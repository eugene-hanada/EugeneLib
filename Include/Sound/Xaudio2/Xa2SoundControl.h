#pragma once
#include <xaudio2.h>
#include <memory>
#include "../SoundBase.h"

namespace Eugene
{

	class SoundControl : 
		public SoundBase
	{
	public:
		SoundControl() = default;
		~SoundControl();
		void SetPan(std::span<float> volumes) final;
		void SetVolume(float volume) final;
		void SetOutput(SoundControl& control) final;

		void* Get(void) noexcept
		{
			return submix_.get();
		}
		void SetOutChannel(std::uint16_t channel) noexcept
		{
			outChannel_ = channel;
		}

		SoundControl(SoundControl&& control) noexcept :
			SoundBase{std::move(control)}, submix_{std::move(control.submix_)}
		{
		}

		SoundControl& operator=(SoundControl&& control) noexcept
		{
			static_cast<SoundBase&>(*this) = control;
			submix_ = std::move(control.submix_);
		}

		void Final() noexcept
		{
			submix_.reset();
		}

		SoundControl(const SoundControl&) = delete;
		SoundControl& operator=(const SoundControl&) = delete;
	private:
		SoundControl(IXAudio2* device, std::uint32_t sample, std::uint32_t stage, std::uint16_t inChannel, std::uint16_t outChannel);

		struct SubmixVoiceDeleter
		{
			void operator()(IXAudio2SubmixVoice* submix)
			{
				submix->DestroyVoice();
			}
		};

		
		std::unique_ptr<IXAudio2SubmixVoice,SubmixVoiceDeleter> submix_;

		friend class Sound;
	};
}
