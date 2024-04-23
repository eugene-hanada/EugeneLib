#include "AaSound3DControl.h"



Eugene::Sound3DControl::Sound3DControlImpl::Sound3DControlImpl(SoundControl& control, SoundControlImpl& impl, std::span<std::uint8_t, 20> handle) :
	impl_{ impl }, control_{ control }
{
}

Eugene::Sound3DControl::Sound3DControlImpl::~Sound3DControlImpl()
{
}

void Eugene::Sound3DControl::Sound3DControlImpl::Set3DSound(
	const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity,
	const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity)
{
}
