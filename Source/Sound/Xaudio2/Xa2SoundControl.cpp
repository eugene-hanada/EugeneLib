#include "Xa2SoundControl.h"
#include <xaudio2.h>
#include "../../../Include/Common/EugeneLibException.h"

Eugene::SoundControl::SoundControlImpl::SoundControlImpl(std::uintptr_t devicePtr, SoundControl& control, std::uint32_t sample, std::uint32_t stage):
	control_{control}
{
	auto xaudio2{ reinterpret_cast<IXAudio2*>(devicePtr) };
	if (FAILED(xaudio2->CreateSubmixVoice(&submix_, control_.inChannel_, sample, XAUDIO2_VOICE_USEFILTER, stage)))
	{
		throw CreateErrorException("サブミックスボイスの作成に失敗");
	}
}

Eugene::SoundControl::SoundControlImpl::~SoundControlImpl()
{
	submix_->DestroyVoice();
}

void* Eugene::SoundControl::SoundControlImpl::Get(void)
{
	return submix_;
}

void Eugene::SoundControl::SoundControlImpl::SetOutChannel(std::uint16_t channel)
{
	control_.outChannel_ = channel;
}

void Eugene::SoundControl::SoundControlImpl::SetPan(std::span<float> volumes)
{
	if ((control_.inChannel_ * control_.outChannel_) + control_.inChannel_  >= volumes.size())
	{
		submix_->SetOutputMatrix(nullptr, control_.inChannel_, control_.outChannel_, volumes.data());
	}
}

void Eugene::SoundControl::SoundControlImpl::SetVolume(float volume)
{
	if (volume  != control_.volume_)
	{
		control_.volume_ = volume;
		submix_->SetVolume(volume  * volume);
	}
}

void Eugene::SoundControl::SoundControlImpl::SetOutput(SoundControl& control)
{
	control_.outChannel_ = control.GetInChannel();
	auto ptr = static_cast<IXAudio2SubmixVoice*>(control.Get());
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	submix_->SetOutputVoices(&sends);
}
