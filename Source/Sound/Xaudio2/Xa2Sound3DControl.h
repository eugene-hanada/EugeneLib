#pragma once
#include "../../../Include/Sound/Sound3DControl.h"
#include <span>

struct IXAudio2;
struct IXAudio2SubmixVoice;

namespace Eugene
{
	class Sound3DControl::Sound3DControlImpl
	{
	public:
		Sound3DControlImpl(SoundControl& control,SoundControlImpl& impl, std::span<std::uint8_t, 20> handle);
		~Sound3DControlImpl();
		void Set3DSound(
			const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity
			, const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity);

		void SetVolume(float volume) ;

		void SetPan(std::span<float> volumes);

		void SetOutput(SoundControl& control);

		void* Get(void);
	private:
		std::span<std::uint8_t, 20> handle_;
		SoundControl& control_;
		SoundControlImpl& impl_;
	};
}
