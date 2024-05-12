#include "AaSound3DControl.h"



Eugene::Aaudio3DControl::Aaudio3DControl(Eugene::SoundBase& soundBase,AaSubmix* sumix, std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel):
        Sound3DControl{sample,inChannel,outChannel,sample},
        submix_{soundBase, sumix,sample}
{
}

Eugene::Aaudio3DControl::~Aaudio3DControl()
{
}

void Eugene::Aaudio3DControl::Aaudio3DControl::Set3DSound(
	const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity,
	const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity)
{
}

void Eugene::Aaudio3DControl::SetOutput(Eugene::SoundControl &control) {
    outChannel_ = control.GetInChannel();
    auto submix = reinterpret_cast<AaSubmix*>(control.Get());
    submix_.SetOut(submix, inChannel_,outChannel_);
}

void Eugene::Aaudio3DControl::SetPan(std::span<float> volumes) {
    submix_.SetOutMatrix(volumes);
}

void Eugene::Aaudio3DControl::SetVolume(float volume) {
    volume_ = volume * volume;
}

void *Eugene::Aaudio3DControl::Get(void) {
    return &submix_;
}
