#include "Xa2Sound3DControl.h"
#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>
#include <x3daudio.h>
#include <vector>
#include "../../../Include/Common/EugeneLibException.h"


Eugene::Xa2Sound3DControl::Xa2Sound3DControl(IXAudio2* xaudio2, std::span<std::uint8_t, 20> handle, std::uint16_t outChannel, std::uint16_t inChannel, std::uint32_t sample) :
	handle_{handle}
{
	inChannel_ = inChannel;
	outChannel_ = outChannel;
	if (FAILED(xaudio2->CreateSubmixVoice(&submix_, inChannel_, sample, XAUDIO2_VOICE_USEFILTER)))
	{
		throw EugeneLibException("3D用サブミックスボイスの作成");
	}


}

Eugene::Xa2Sound3DControl::~Xa2Sound3DControl()
{
	submix_->DestroyVoice();
}

void Eugene::Xa2Sound3DControl::Update3DSound(
	const Vector3& listenerFront, const Vector3& listenerTop, const Vector3& listenerPos, const Vector3& listenerVeclocity,
	const Vector3& emitterFront, const Vector3& emitterTop, const Vector3& emitterPos, const Vector3& emitterVelocity)
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
	dsp.SrcChannelCount = inChannel_;
	dsp.DstChannelCount = outChannel_;
	std::vector<float>  matrix(outChannel_ * inChannel_);
	dsp.pMatrixCoefficients = matrix.data();
	
	X3DAudioCalculate(
		handle_.data(),
		&listener,
		&emitter,
		X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB,
		&dsp
	);

	submix_->SetOutputMatrix(nullptr, inChannel_, dsp.DstChannelCount, dsp.pMatrixCoefficients);
	
	XAUDIO2_FILTER_PARAMETERS filter{ LowPassFilter, 2.0f * std::sin(X3DAUDIO_PI / 6.0f * dsp.LPFDirectCoefficient), 1.0f };
	XAUDIO2_VOICE_DETAILS details;
	submix_->GetVoiceDetails(&details);
	filter.OneOverQ = XAudio2CutoffFrequencyToOnePoleCoefficient(filter.Frequency, details.InputSampleRate);
	submix_->SetFilterParameters(&filter);
	//submix_->SetOutputFilterParameters(nullptr, & filter);
}

void Eugene::Xa2Sound3DControl::SetVolume(float volume)
{
	if (volume * volume != volume_)
	{
		volume_ = volume * volume;
		submix_->SetVolume(volume_);
	}
}

void* Eugene::Xa2Sound3DControl::Get(void)
{
	return submix_;
}
