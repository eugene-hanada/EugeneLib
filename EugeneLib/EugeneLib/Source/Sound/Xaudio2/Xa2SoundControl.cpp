#include "Xa2SoundControl.h"
#include <xaudio2.h>
#include "../../../Include/Common/EugeneLibException.h"

EugeneLib::Xa2SoundControl::Xa2SoundControl(IXAudio2* xaudio2, std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel)
{

	if (FAILED(xaudio2->CreateSubmixVoice(&submix_, inputChannel, sample, XAUDIO2_VOICE_USEFILTER)))
	{
		throw EugeneLibException("サブミックスボイスの作成に失敗");
	}

	inChannel_ = inputChannel;
	outChannel_ = outChannel;
}

void* EugeneLib::Xa2SoundControl::Get(void)
{
	return submix_;
}

void EugeneLib::Xa2SoundControl::SetPan(std::span<float> volumes)
{
	if (outChannel_ == volumes.size())
	{
		submix_->SetOutputMatrix(nullptr, inChannel_, outChannel_, volumes.data());
	}
}

void EugeneLib::Xa2SoundControl::SetVolume(float volume)
{
	if (volume * volume != volume_)
	{
		volume_ = volume * volume;
		submix_->SetVolume(volume_);
	}
}
