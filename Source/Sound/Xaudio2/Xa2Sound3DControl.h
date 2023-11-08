#pragma once
#include "../../../Include/Sound/Sound3DControl.h"
#include <span>

struct IXAudio2;
struct IXAudio2SubmixVoice;

namespace Eugene
{
	class Xa2Sound3DControl :
		public Sound3DControl
	{
	public:
		Xa2Sound3DControl(IXAudio2* xaudio2, std::span<std::uint8_t, 20> handle,std::uint16_t outChannel, std::uint16_t inChannel, std::uint32_t sample, std::uint32_t stage);
		~Xa2Sound3DControl();
	private:
		void Set3DSound(
			const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity
			, const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity) final;

		void SetVolume(float volume) final;

		void SetPan(std::span<float> volumes) final
		{}

		void SetOutput(SoundControl& control) final;

		void* Get(void) final;

		std::span<std::uint8_t, 20> handle_;

		IXAudio2SubmixVoice* submix_;
	};
}
