#include "Xa2SoundControl.h"
#include <xaudio2.h>
#include "../../../Include/Common/EugeneLibException.h"

Eugene::Xaudio2Control::Xaudio2Control(IXAudio2* xaudio2,std::uint32_t sample, std::uint32_t stage, std::uint16_t inChannel, std::uint16_t outChannel):
	SoundControl{sample,inChannel,outChannel,stage}
{
	if (FAILED(xaudio2->CreateSubmixVoice(std::out_ptr(submix_), inChannel_, sample, XAUDIO2_VOICE_USEFILTER, stage)))
	{
		throw CreateErrorException("サブミックスボイスの作成に失敗");
	}
}

Eugene::Xaudio2Control::~Xaudio2Control()
{
}

void* Eugene::Xaudio2Control::Xaudio2Control::Get(void)
{
	return submix_.get();
}

void Eugene::Xaudio2Control::Xaudio2Control::SetOutChannel(std::uint16_t channel)
{
	outChannel_ = channel;
}

void Eugene::Xaudio2Control::Xaudio2Control::SetPan(std::span<float> volumes)
{
	if ((inChannel_ * outChannel_) <= volumes.size())
	{
		submix_->SetOutputMatrix(nullptr, inChannel_, outChannel_, volumes.data());
	}
}

void Eugene::Xaudio2Control::Xaudio2Control::SetVolume(float volume)
{
	if (volume  != volume_)
	{
		volume_ = volume;
		submix_->SetVolume(volume  * volume);
	}
}

void Eugene::Xaudio2Control::SetOutput(SoundControl& control)
{
	outChannel_ = control.GetInChannel();
	auto ptr = static_cast<IXAudio2SubmixVoice*>(control.Get());
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	submix_->SetOutputVoices(&sends);
}
