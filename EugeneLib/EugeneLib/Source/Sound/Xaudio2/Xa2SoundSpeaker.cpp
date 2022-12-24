#include "Xa2SoundSpeaker.h"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <algorithm>
#include "../../../Include/Sound/Wave.h"
#include "../../../Include/Common/EugeneLibException.h"


EugeneLib::Xa2SoundSpeaker::Xa2SoundSpeaker(IXAudio2* xaudio2, const Wave& wave, std::uint16_t outChannel)
{
	WAVEFORMATEX format{
		wave.GetFmt().type,
		wave.GetFmt().channel,
		wave.GetFmt().sample,
		wave.GetFmt().byte,
		wave.GetFmt().block,
		wave.GetFmt().bit,
		0u
	};

	if (FAILED(xaudio2->CreateSourceVoice(&source_, &format)))
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
	inChannel_ = format.nChannels;
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
