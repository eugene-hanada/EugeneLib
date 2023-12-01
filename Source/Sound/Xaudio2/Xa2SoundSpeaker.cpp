#include "Xa2SoundSpeaker.h"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <algorithm>
#include "../../../Include/Sound/SoundFile.h"
#include "../../../Include/Sound/SoundControl.h"
#include "../../../Include/Common/EugeneLibException.h"




//Eugene::Xa2SoundSpeaker::Xa2SoundSpeaker(IXAudio2* xaudio2, const Wave& wave, std::uint16_t outChannel, const float maxPitchRate) :
//	SoundSpeaker{maxPitchRate}
//{
//	WAVEFORMATEXTENSIBLE formatEx;
//	formatEx.Format = {
//		wave.GetFmt().type,
//		wave.GetFmt().channel,
//		wave.GetFmt().sample,
//		wave.GetFmt().byte,
//		wave.GetFmt().block,
//		wave.GetFmt().bit,
//		wave.GetFmt().ex
//	};
//	formatEx.Samples.wReserved = wave.GetEx().reserved;
//	formatEx.dwChannelMask = wave.GetEx().channelMask;
//	formatEx.SubFormat.Data1 = wave.GetEx().d1;
//	formatEx.SubFormat.Data2 = wave.GetEx().d2;
//	formatEx.SubFormat.Data3 = wave.GetEx().d3;
//	std::copy(std::begin(wave.GetEx().d4), std::end(wave.GetEx().d4), formatEx.SubFormat.Data4);
//
//	if (FAILED(xaudio2->CreateSourceVoice(&source_, &formatEx.Format, 0, maxPitchRate)))
//	{
//		throw EugeneLibException("ソースボイス生成失敗");
//	}
//	buffer_ = std::make_unique<XAUDIO2_BUFFER>();
//	buffer_->Flags = XAUDIO2_END_OF_STREAM;
//	buffer_->pAudioData = wave.GetData().data();
//	buffer_->AudioBytes = static_cast<unsigned int>(wave.GetData().size() * sizeof(wave.GetData()[0]));
//	buffer_->LoopCount = 0;
//
//	if (buffer_->LoopLength > 0)
//	{
//		buffer_->LoopCount = 1;
//	}
//
//	if(FAILED(source_->SubmitSourceBuffer(buffer_.get())))
//	{
//		throw EugeneLibException("ソースボイス生成失敗");
//	}
//
//	outChannel_ = outChannel;
//	inChannel_ = formatEx.Format.nChannels;
//}
//
//Eugene::Xa2SoundSpeaker::Xa2SoundSpeaker(IXAudio2* xaudio2, const OggVorbis& ogg, std::uint16_t outChannel, const float maxPitchRate) :
//	SoundSpeaker{maxPitchRate}
//{
//	WAVEFORMATEX format; 
//	auto oggFormat = ogg.GetFormat();
//
//	format.wFormatTag = WAVE_FORMAT_PCM;
//	format.nChannels = oggFormat.channel;
//	format.cbSize = 0;
//	format.nSamplesPerSec = oggFormat.sample;
//	format.nAvgBytesPerSec = oggFormat.byte;
//	format.nBlockAlign = oggFormat.block;
//	format.wBitsPerSample = oggFormat.bit;
//
//	if (FAILED(xaudio2->CreateSourceVoice(&source_, &format, 0, maxPitchRate)))
//	{
//		throw EugeneLibException("ソースボイス生成失敗");
//	}
//	buffer_ = std::make_unique<XAUDIO2_BUFFER>();
//	buffer_->Flags = XAUDIO2_END_OF_STREAM;
//	buffer_->pAudioData = ogg.GetDataPtr();
//	buffer_->AudioBytes = static_cast<unsigned int>(ogg.GetDataSize());
//	buffer_->LoopCount = 0;
//
//	if (buffer_->LoopLength > 0)
//	{
//		buffer_->LoopCount = 1;
//	}
//
//	if (FAILED(source_->SubmitSourceBuffer(buffer_.get())))
//	{
//		throw EugeneLibException("ソースボイス生成失敗");
//	}
//
//	outChannel_ = outChannel;
//	inChannel_ = format.nChannels;
//}

Eugene::SoundSpeaker::SoundSpeakerImpl::SoundSpeakerImpl(std::uintptr_t devicePtr, SoundSpeaker& speaker, const SoundFile& soundFile) :
	speaker_{speaker}
{
	auto xaudio2{ reinterpret_cast<IXAudio2*>(devicePtr) };
	// フォーマットをセット
	const auto& format = soundFile.GetFormat();
	const auto ext = soundFile.GetFormatExt();
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
	if (FAILED(xaudio2->CreateSourceVoice(std::out_ptr(source_), &formatEx.Format, 0, speaker_.maxPitchRate_)))
	{
		throw CreateErrorException("ソースボイス生成失敗");
	}

	// バッファも用意
	buffer_ = std::make_unique<XAUDIO2_BUFFER>();

	speaker_.inChannel_ = formatEx.Format.nChannels;
}



Eugene::SoundSpeaker::SoundSpeakerImpl::~SoundSpeakerImpl()
{
	source_->Stop();
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::Play(int loopCount)
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

void Eugene::SoundSpeaker::SoundSpeakerImpl::Stop(void)
{
	source_->Stop(XAUDIO2_PLAY_TAILS);
}

bool Eugene::SoundSpeaker::SoundSpeakerImpl::IsEnd(void) const
{
	XAUDIO2_VOICE_STATE state;
	source_->GetState(&state);
	return state.BuffersQueued <= 0;
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::SetPitchRate(float rate)
{
	source_->SetFrequencyRatio(std::clamp(rate, XAUDIO2_MIN_FREQ_RATIO, XAUDIO2_MAX_FREQ_RATIO));
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::SetVolume(float volume)
{
	if (volume != speaker_.volume_)
	{
		source_->SetVolume(volume * volume);
		speaker_.volume_ = volume;
	}
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::SetPan(std::span<float> volumes)
{
	if ((speaker_.inChannel_ * speaker_.outChannel_) + speaker_.inChannel_ >= volumes.size())
	{
		source_->SetOutputMatrix(nullptr, speaker_.inChannel_, speaker_.outChannel_, volumes.data());
	}
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::SetOutput(SoundControl& control)
{
	speaker_.outChannel_ = control.GetInChannel();
	auto ptr = static_cast<IXAudio2SubmixVoice*>(control.Get());
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	source_->SetOutputVoices(&sends);
}

void Eugene::SoundSpeaker::SoundSpeakerImpl::SetData(const std::uint8_t* ptr, const std::uint64_t size)
{
	buffer_->Flags = XAUDIO2_END_OF_STREAM;
	buffer_->pAudioData = ptr;
	buffer_->AudioBytes = static_cast<unsigned int>(size);
	buffer_->LoopCount = 0;
}
