#include "Xa2SoundSpeaker.h"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <algorithm>
#include "../../../Include/Sound/Wave.h"
#include "../../../Include/Sound/SoundControl.h"
#include "../../../Include/Common/EugeneLibException.h"


EugeneLib::Xa2SoundSpeaker::Xa2SoundSpeaker(IXAudio2* xaudio2, const Wave& wave, std::uint16_t outChannel, const float maxPitchRate) :
	SoundSpeaker{maxPitchRate}
{
	WAVEFORMATEXTENSIBLE formatEx;
	formatEx.Format = {
		wave.GetFmt().type,
		wave.GetFmt().channel,
		wave.GetFmt().sample,
		wave.GetFmt().byte,
		wave.GetFmt().block,
		wave.GetFmt().bit,
		wave.GetFmt().ex
	};
	formatEx.Samples.wReserved = wave.GetEx().reserved;
	formatEx.dwChannelMask = wave.GetEx().channelMask;
	formatEx.SubFormat.Data1 = wave.GetEx().d1;
	formatEx.SubFormat.Data2 = wave.GetEx().d2;
	formatEx.SubFormat.Data3 = wave.GetEx().d3;
	std::copy(std::begin(wave.GetEx().d4), std::end(wave.GetEx().d4), formatEx.SubFormat.Data4);

	struct WaveData
	{
		WAVEFORMATEX* formatPtr;
		const std::uint8_t* startPtr;
	};
	WaveData d;
	d.formatPtr = &formatEx.Format;

	if (FAILED(xaudio2->CreateSourceVoice(&source_, &formatEx.Format, 0, maxPitchRate)))
	{
		throw EugeneLibException("ソースボイス生成失敗");
	}
	buffer_ = std::make_unique<XAUDIO2_BUFFER>();
	buffer_->Flags = XAUDIO2_END_OF_STREAM;
	buffer_->pAudioData = wave.GetData().data();
	buffer_->AudioBytes = static_cast<unsigned int>(wave.GetData().size() * sizeof(wave.GetData()[0]));
	buffer_->LoopCount = 0;

	if (buffer_->LoopLength > 0)
	{
		buffer_->LoopCount = 1;
	}

	if(FAILED(source_->SubmitSourceBuffer(buffer_.get())))
	{
		throw EugeneLibException("ソースボイス生成失敗");
	}

	outChannel_ = outChannel;
	inChannel_ = formatEx.Format.nChannels;
}

EugeneLib::Xa2SoundSpeaker::~Xa2SoundSpeaker()
{
	source_->Stop();
	source_->DestroyVoice();
}

void EugeneLib::Xa2SoundSpeaker::Play(void) const
{
	if (FAILED(source_->FlushSourceBuffers())|| 
		FAILED(source_->SubmitSourceBuffer(buffer_.get())) ||
		FAILED(source_->Start()))
	{
		throw EugeneLibException("failedPlay");
	}
}

void EugeneLib::Xa2SoundSpeaker::Stop(void) const
{
	source_->Stop(XAUDIO2_PLAY_TAILS);
}

bool EugeneLib::Xa2SoundSpeaker::IsEnd(void) const
{
	XAUDIO2_VOICE_STATE state;
	source_->GetState(&state);
	return state.BuffersQueued <= 0;
}

void EugeneLib::Xa2SoundSpeaker::SetPitchRate(float rate)
{
	source_->SetFrequencyRatio(std::clamp(rate, XAUDIO2_MIN_FREQ_RATIO, XAUDIO2_MAX_FREQ_RATIO));
}

void EugeneLib::Xa2SoundSpeaker::SetVolume(float volume)
{
	source_->SetVolume(volume * volume);
}

void EugeneLib::Xa2SoundSpeaker::SetPan(std::span<float> volumes)
{
	if (outChannel_ == volumes.size())
	{
		source_->SetOutputMatrix(nullptr, inChannel_, outChannel_, volumes.data());
	}
}

void EugeneLib::Xa2SoundSpeaker::SetOutput(SoundControl& control)
{
	auto ptr = static_cast<IXAudio2SubmixVoice*>(control.Get());
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	source_->SetOutputVoices(&sends);
}
