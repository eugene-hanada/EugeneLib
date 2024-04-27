#include "../../Include/Sound/Sound3DControl.h"

#ifdef USE_WINDOWS
#include "Xaudio2/Xa2Sound3DControl.h"
#include "Xaudio2/Xa2SoundControl.h"
#elif USE_ANDROID
#include "AAudio/AaSound3DControl.h"
#include "AAudio/AaSoundControl.h"
#endif

Eugene::Sound3DControl::~Sound3DControl()
{
}

void Eugene::Sound3DControl::Set3DSound(const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity, const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity)
{
}

Eugene::Sound3DControl::Sound3DControl(
	std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t stage
):
	SoundControl{sample, inChannel,outChannel,stage}
{
}