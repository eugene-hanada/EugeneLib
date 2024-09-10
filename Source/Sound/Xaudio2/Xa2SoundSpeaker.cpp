#include "../../../Include/Sound/Xaudio2/Xa2SoundSpeaker.h"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <algorithm>
#include "../../../Include/Sound/SoundFile.h"
#include "../../../Include/Sound/SoundControl.h"
#include "../../../Include/Utils/EugeneLibException.h"


Eugene::SoundSpeaker::SoundSpeaker(IXAudio2* xaudio2, const SoundFile& soundFile, std::uint16_t outChannel, const float maxPitchRate) :
	maxPitchRate_{maxPitchRate}
{
	outChannel_ = outChannel;
	// フォーマットをセット
	const auto& format{ soundFile.GetFormat() };
	const auto ext{ soundFile.GetFormatExt() };
	WAVEFORMATEXTENSIBLE formatEx;
	formatEx.Format = {
		format.type,
		format.channel,
		format.sample,
		format.byte,
		format.block,
		format.bit,
		format.ex
	};
	formatEx.Samples.wReserved = ext.reserved;
	formatEx.dwChannelMask = ext.channelMask;
	formatEx.SubFormat.Data1 = ext.d1;
	formatEx.SubFormat.Data2 = ext.d2;
	formatEx.SubFormat.Data3 = ext.d3;
	std::copy(std::begin(ext.d4), std::end(ext.d4), formatEx.SubFormat.Data4);

	// ソースボイス生成
	if (FAILED(xaudio2->CreateSourceVoice(std::out_ptr(source_), &formatEx.Format, 0, maxPitchRate_)))
	{
		throw EugeneLibException("ソースボイス生成失敗");
	}

	// バッファも用意
	buffer_ = std::make_unique<XAUDIO2_BUFFER>();

	inChannel_ = formatEx.Format.nChannels;
}



Eugene::SoundSpeaker::~SoundSpeaker()
{
	if (source_)
	{
		source_->Stop(XAUDIO2_PLAY_TAILS);
	}
}

void Eugene::SoundSpeaker::Play(int loopCount)
{
	buffer_->LoopCount = loopCount == -1 ? XAUDIO2_LOOP_INFINITE : static_cast<std::uint32_t>(loopCount);
	if (FAILED(source_->Stop(XAUDIO2_PLAY_TAILS)) ||
		FAILED(source_->FlushSourceBuffers())|| 
		FAILED(source_->SubmitSourceBuffer(buffer_.get())) ||
		FAILED(source_->Start()))
	{
		throw EugeneLibException("failedPlay");
	}
}

void Eugene::SoundSpeaker::Stop(void) noexcept
{
	source_->Stop(XAUDIO2_PLAY_TAILS);
}

bool Eugene::SoundSpeaker::IsEnd(void) const
{
	XAUDIO2_VOICE_STATE state;
	source_->GetState(&state);
	return state.BuffersQueued <= 0;
}

void Eugene::SoundSpeaker::SetPitchRate(float rate)
{
	source_->SetFrequencyRatio(std::clamp(rate, XAUDIO2_MIN_FREQ_RATIO, XAUDIO2_MAX_FREQ_RATIO));
}

void Eugene::SoundSpeaker::SetVolume(float volume)
{
	if (volume != volume_)
	{
		source_->SetVolume(volume * volume);
		volume_ = volume;
	}
}

void Eugene::SoundSpeaker::SetPan(std::span<float> volumes)
{
	if ((inChannel_ * outChannel_) >= volumes.size())
	{
		source_->SetOutputMatrix(nullptr, inChannel_, outChannel_, volumes.data());
	}
}

void Eugene::SoundSpeaker::SetOutput(SoundControl& control)
{
	outChannel_ = control.GetInChannel();
	auto ptr{ static_cast<IXAudio2SubmixVoice*>(control.Get()) };
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	auto A = source_->SetOutputVoices(&sends);
}

void Eugene::SoundSpeaker::SetData(const std::uint8_t* ptr, const std::uint64_t size)
{
	buffer_->Flags = XAUDIO2_END_OF_STREAM;
	buffer_->pAudioData = ptr;
	buffer_->AudioBytes = static_cast<unsigned int>(size);
	buffer_->LoopCount = 0;
}
