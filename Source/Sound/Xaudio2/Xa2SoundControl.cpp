#include "../../../Include/Sound/Xaudio2/Xa2SoundControl.h"
#include <xaudio2.h>
#include "../../../Include/Utils/EugeneLibException.h"

Eugene::SoundControl::SoundControl(IXAudio2* xaudio2,std::uint32_t sample, std::uint32_t stage, std::uint16_t inChannel, std::uint16_t outChannel)
{
	inChannel_ = inChannel;
	outChannel_ = outChannel;
	if (FAILED(xaudio2->CreateSubmixVoice(std::out_ptr(submix_), inChannel_, sample, XAUDIO2_VOICE_USEFILTER, stage)))
	{
		throw EugeneLibException("サブミックスボイスの作成に失敗");
	}
}

Eugene::SoundControl::~SoundControl()
{
}

void Eugene::SoundControl::SoundControl::SetPan(std::span<float> volumes)
{
	if ((inChannel_ * outChannel_) <= volumes.size())
	{
		submix_->SetOutputMatrix(nullptr, inChannel_, outChannel_, volumes.data());
	}
}

void Eugene::SoundControl::SoundControl::SetVolume(float volume)
{
	if (volume  != volume_)
	{
		volume_ = volume;
		submix_->SetVolume(volume  * volume);
	}
}

void Eugene::SoundControl::SetOutput(SoundControl& control)
{
	outChannel_ = control.GetInChannel();
	auto ptr{ static_cast<IXAudio2SubmixVoice*>(control.Get()) };
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	submix_->SetOutputVoices(&sends);
}
