#include "Xa2SoundSpeaker.h"
#include <xaudio2.h>
#include "../../../Include/Sound/Wave.h"
#include "../../../Include/Common/EugeneLibException.h"

EugeneLib::Xa2SoundSpeaker::Xa2SoundSpeaker(IXAudio2* xaudio2, const Wave& wave)
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

	if(FAILED(source_->SubmitSourceBuffer(buffer_.get())))
	{
		throw EugeneLibException("ソースボイス生成失敗");
	}
}

EugeneLib::Xa2SoundSpeaker::~Xa2SoundSpeaker()
{
	source_->DestroyVoice();
}

void EugeneLib::Xa2SoundSpeaker::Play(void) const
{
	if (FAILED(source_->Start()))
	{
		throw EugeneLibException("failedPlay");
	}

	
}

void EugeneLib::Xa2SoundSpeaker::Stop(void) const
{
	source_->Stop();
}

bool EugeneLib::Xa2SoundSpeaker::IsEnd(void) const
{
	XAUDIO2_VOICE_STATE state;
	source_->GetState(&state);
	return state.BuffersQueued <= 0;
}
