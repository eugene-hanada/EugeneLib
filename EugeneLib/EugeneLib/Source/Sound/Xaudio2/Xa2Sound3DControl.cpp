#include "Xa2Sound3DControl.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include <vector>

void EugeneLib::Xa2Sound3DControl::Update3DSound(
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

	listener.OrientFront = X3DAUDIO_VECTOR{ listenerFront.x,listenerFront.y,listenerFront.z };
	listener.OrientTop = X3DAUDIO_VECTOR{ listenerTop.x,listenerTop.y,listenerTop.z };
	listener.Position = X3DAUDIO_VECTOR{ listenerPos.x,listenerPos.y,listenerPos.z };
	listener.Velocity = X3DAUDIO_VECTOR{ listenerVeclocity.x,listenerVeclocity.y,listenerVeclocity.z };

	X3DAUDIO_DSP_SETTINGS dsp{};
	dsp.SrcChannelCount = inChannel_;
	dsp.DstChannelCount = outChannel_;
	std::vector<float> matrix(outChannel_);
	dsp.pMatrixCoefficients = matrix.data();
	X3DAudioCalculate(
		handle_.data(),
		&listener,
		&emitter,
		X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB,
		&dsp
	);

	submix_->SetOutputMatrix(nullptr, inChannel_, outChannel_, &dsp.ReverbLevel);
	XAUDIO2_FILTER_PARAMETERS filter{ LowPassFilter, 2.0f * std::sin(X3DAUDIO_PI / 6.0f * dsp.LPFDirectCoefficient) };
	submix_->SetFilterParameters(&filter);
	
}
