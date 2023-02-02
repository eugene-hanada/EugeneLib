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
		Xa2Sound3DControl(IXAudio2* xaudio2, std::span<std::uint8_t, 20> handle,std::uint16_t outChannel, std::uint16_t inChannel, std::uint32_t sample);
		~Xa2Sound3DControl();
	private:
		void Update3DSound(
			const Vector3& listenerFront, const Vector3& listenerTop, const Vector3& listenerPos, const Vector3& listenerVeclocity
			, const Vector3& emitterFront, const Vector3& emitterTop, const Vector3& emitterPos, const Vector3& emitterVelocity) final;

		void SetVolume(float volume) final;

		void SetPan(std::span<float> volumes) final
		{}

		void* Get(void) final;

		std::span<std::uint8_t, 20> handle_;

		IXAudio2SubmixVoice* submix_;
	};
}
