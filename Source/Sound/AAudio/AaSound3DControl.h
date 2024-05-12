#pragma once
#include "../../../Include/Sound/Sound3DControl.h"
#include <span>
#include "AaSubmix.h"

namespace Eugene
{
	class Aaudio3DControl : public Sound3DControl
	{
	public:
        Aaudio3DControl(Eugene::SoundBase& soundBase,AaSubmix* sumix, std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel);
		~Aaudio3DControl();
        void* Get(void) final;
        void SetVolume(float volume) final;
        void SetPan(std::span<float> volumes) final;
        void SetOutput(SoundControl& control) final;
		void Set3DSound(
			const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity
			, const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity);
	private:
        AaSubmix submix_;
	};
}
