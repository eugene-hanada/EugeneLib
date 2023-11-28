#include "Xa2Sound3DControl.h"
#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>
#include <x3daudio.h>
#include <vector>
#include <cmath>
#include "../../../Include/Common/EugeneLibException.h"
#include "Xa2SoundControl.h"


Eugene::Sound3DControl::Sound3DControlImpl::Sound3DControlImpl(SoundControl& control,SoundControlImpl& impl, std::span<std::uint8_t, 20> handle) :
	handle_{handle}, impl_{impl}, control_{control}
{
}

Eugene::Sound3DControl::Sound3DControlImpl::~Sound3DControlImpl()
{
}

void Eugene::Sound3DControl::Sound3DControlImpl::Set3DSound(
	const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity,
	const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity)
{
	X3DAUDIO_EMITTER emitter{};
	X3DAUDIO_LISTENER listener{};
	emitter.OrientFront = X3DAUDIO_VECTOR{ emitterFront.x,emitterFront.y,emitterFront.z };
	emitter.OrientTop = X3DAUDIO_VECTOR{ emitterTop.x,emitterTop.y,emitterTop.z };
	emitter.Position = X3DAUDIO_VECTOR{ emitterPos.x,emitterPos.y,emitterPos.z };
	emitter.Velocity = X3DAUDIO_VECTOR{ emitterVelocity.x,emitterVelocity.y,emitterVelocity.z };
	emitter.ChannelCount = 1;
	emitter.CurveDistanceScaler = emitter.DopplerScaler = 1.0f;
	emitter.pChannelAzimuths = nullptr;
	listener.OrientFront = X3DAUDIO_VECTOR{ listenerFront.x,listenerFront.y,listenerFront.z };
	listener.OrientTop = X3DAUDIO_VECTOR{ listenerTop.x,listenerTop.y,listenerTop.z };
	listener.Position = X3DAUDIO_VECTOR{ listenerPos.x,listenerPos.y,listenerPos.z };
	listener.Velocity = X3DAUDIO_VECTOR{ listenerVeclocity.x,listenerVeclocity.y,listenerVeclocity.z };

	X3DAUDIO_DSP_SETTINGS dsp{};
	dsp.SrcChannelCount = control_.inChannel_;
	dsp.DstChannelCount = control_.outChannel_;
	std::vector<float>  matrix(outChannel_ * inChannel_);
	dsp.pMatrixCoefficients = matrix.data();
	
	X3DAudioCalculate(
		handle_.data(),
		&listener,
		&emitter,
		X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB,
		&dsp
	);

	impl_.
	submix_->SetOutputMatrix(nullptr, inChannel_, dsp.DstChannelCount, dsp.pMatrixCoefficients);
	
	XAUDIO2_FILTER_PARAMETERS filter{ LowPassFilter, 2.0f * std::sin(X3DAUDIO_PI / 6.0f * dsp.LPFDirectCoefficient), 1.0f };
	XAUDIO2_VOICE_DETAILS details;
	submix_->GetVoiceDetails(&details);
	//filter.OneOverQ = XAudio2CutoffFrequencyToOnePoleCoefficient(filter.Frequency, details.InputSampleRate);
	submix_->SetFilterParameters(&filter);

}

void Eugene::Sound3DControl::Sound3DControlImpl::SetVolume(float volume)
{
	if (volume != volume_)
	{
		volume_ = volume;
		submix_->SetVolume(volume * volume);
	}
}

void Eugene::Sound3DControl::Sound3DControlImpl::SetOutput(SoundControl& control)
{
	outChannel_ = control.GetInChannel();
	auto ptr = static_cast<IXAudio2SubmixVoice*>(control.Get());
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	submix_->SetOutputVoices(&sends);
}

void* Eugene::Sound3DControl::Sound3DControlImpl::Get(void)
{
	return submix_;
}
