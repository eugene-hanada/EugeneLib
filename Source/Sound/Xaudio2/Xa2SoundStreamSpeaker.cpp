#include "Xa2SoundStreamSpeaker.h"
#include "../../../Include/Sound/SoundCommon.h"
#include "../../../Include/Sound/Wave.h"





Eugene::Xa2SoundStreamSpeaker::Xa2SoundStreamSpeaker(IXAudio2* device, const std::filesystem::path& path, std::uint16_t outChannel)
{
	collback_ = std::make_unique<CollBack>(*this);
	file_.open(path, std::ios::binary);
	Wave::RIFF riff;
	file_.read(reinterpret_cast<char*>(&riff), sizeof(riff));
	int id = 0;
	auto pos = file_.tellg();
	while (true)
	{
		file_.read(reinterpret_cast<char*>(&id), 4);
		if (Wave::fmt == id)
		{
			break;
		}
		
	}

	WAVEFORMATEXTENSIBLE format;
	file_.read(reinterpret_cast<char*>(&format.Format), sizeof(format.Format));
	if (format.Format.wFormatTag == 1)
	{
		file_.read(reinterpret_cast<char*>(&format.Samples), sizeof(format.Samples));
		file_.read(reinterpret_cast<char*>(&format.dwChannelMask), sizeof(format.dwChannelMask));
		file_.read(reinterpret_cast<char*>(&format.SubFormat), sizeof(format.SubFormat));
	}
	file_.seekg(pos);
	while (true)
	{
		file_.read(reinterpret_cast<char*>(&id), 4);
		if (Wave::data == id)
		{
			break;
		}

	}

	device->CreateSourceVoice(&source_, &format.Format, 0, 2.0f, collback_.get());

	bufferData_.resize(format.Format.nAvgBytesPerSec);
	streamData_.resize(format.Format.nAvgBytesPerSec);
	bytesPerSec = format.Format.nAvgBytesPerSec;
	file_.read(reinterpret_cast<char*>(bufferData_.data()), bufferData_.size());
	//file_.read(reinterpret_cast<char*>(streamData_.data()), streamData_.size());

	buffer_->AudioBytes = format.Format.nAvgBytesPerSec;
	buffer_->pAudioData = bufferData_.data();
	buffer_->LoopCount = 0;
	buffer_->Flags = XAUDIO2_END_OF_STREAM;

	//streamBuffer_->AudioBytes = format.Format.nAvgBytesPerSec;
	//streamBuffer_->pAudioData = streamData_.data();
	//streamBuffer_->LoopCount = 0;
	//streamBuffer_->Flags = XAUDIO2_END_OF_STREAM;


}

void Eugene::Xa2SoundStreamSpeaker::Play(void) const
{
}

void Eugene::Xa2SoundStreamSpeaker::Stop(void) const
{
}

bool Eugene::Xa2SoundStreamSpeaker::IsEnd(void) const
{
	return false;
}

void Eugene::Xa2SoundStreamSpeaker::SetPitchRate(float rate)
{
}

void Eugene::Xa2SoundStreamSpeaker::SetOutput(SoundControl& control)
{
}

void Eugene::Xa2SoundStreamSpeaker::SetVolume(float volume)
{
}

void Eugene::Xa2SoundStreamSpeaker::SetPan(std::span<float> volumes)
{
}

void Eugene::Xa2SoundStreamSpeaker::Worker(void)
{
	while (isRun_.load())
	{
		file_.read(reinterpret_cast<char*>(bufferData_.data()), bufferData_.size());
		buffer_->pAudioData = bufferData_.data();

		semaphore_.acquire();
	}
}

Eugene::Xa2SoundStreamSpeaker::CollBack::CollBack(Xa2SoundStreamSpeaker& speaker) :
	speaker_{speaker}
{
}

void Eugene::Xa2SoundStreamSpeaker::CollBack::OnBufferEnd(void* pBufferContext) noexcept
{
	speaker_.semaphore_.release();
}

void Eugene::Xa2SoundStreamSpeaker::CollBack::OnBufferStart(void* pBufferContext) noexcept
{
}

void Eugene::Xa2SoundStreamSpeaker::CollBack::OnLoopEnd(void* pBufferContext) noexcept
{
}

void Eugene::Xa2SoundStreamSpeaker::CollBack::OnStreamEnd() noexcept
{
}

void Eugene::Xa2SoundStreamSpeaker::CollBack::OnVoiceError(void* pBufferContext, HRESULT Error) noexcept
{
}

void Eugene::Xa2SoundStreamSpeaker::CollBack::OnVoiceProcessingPassEnd() noexcept
{
}

void Eugene::Xa2SoundStreamSpeaker::CollBack::OnVoiceProcessingPassStart(std::uint32_t BytesRequired) noexcept
{
}
