#include "../../Include/Sound/Sound3DControl.h"
#include "Xaudio2/Xa2Sound3DControl.h"
#include "Xaudio2/Xa2SoundControl.h"

Eugene::Sound3DControl::~Sound3DControl()
{
}

void Eugene::Sound3DControl::Set3DSound(const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity, const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity)
{
	impl3D_->Set3DSound(
		listenerFront, listenerTop, listenerPos, listenerVeclocity,
		emitterFront, emitterTop, emitterPos, emitterVelocity
	);
}

Eugene::Sound3DControl::Sound3DControl(
	std::uintptr_t devicePtr,
	std::uint32_t sample,
	std::uint16_t inChannel, 
	std::uint16_t outChannel, 
	std::uint32_t stage, 
	std::uintptr_t handle
):
	SoundControl{devicePtr, sample, inChannel,outChannel,stage}
{
	auto spanHandle = *reinterpret_cast<std::span<std::uint8_t,20u>*>(handle);
	impl3D_ = std::make_unique<Sound3DControlImpl>(*this, *impl_, spanHandle);
}