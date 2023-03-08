#include "Xa2SoundStreamSpeaker.h"
#include "../../../Include/Sound/SoundCommon.h"
#include "../../../Include/Sound/Wave.h"





Eugene::Xa2SoundStreamSpeaker::Xa2SoundStreamSpeaker(IXAudio2* device, const std::filesystem::path& path, std::uint16_t outChannel)
{
	isPlay_.store(false);
	isRun_.store(true);

	collback_ = std::make_unique<CollBack>(*this);
	file_.open(path, std::ios::binary);
	Wave::RIFF riff;
	file_.read(reinterpret_cast<char*>(&riff), sizeof(riff));
	int id = 0;

	// fmt�`�����N��������
	while (true)
	{
		file_.read(reinterpret_cast<char*>(&id), 4);
		if (Wave::fmt == id)
		{
			break;
		}
		
	}

	WAVEFORMATEXTENSIBLE format;
	file_.ignore(4);
	file_.read(reinterpret_cast<char*>(&format.Format), sizeof(format.Format));
	if (format.Format.wFormatTag != 1)
	{
		file_.read(reinterpret_cast<char*>(&format.Samples), sizeof(format.Samples));
		file_.read(reinterpret_cast<char*>(&format.dwChannelMask), sizeof(format.dwChannelMask));
		file_.read(reinterpret_cast<char*>(&format.SubFormat), sizeof(format.SubFormat));
	}
	else
	{
		auto now = file_.tellg();
		now -= 2ull;
		file_.seekg(now);
	}
	
	// �f�[�^�`�����N��������
	while (true)
	{
		file_.read(reinterpret_cast<char*>(&id), 4);
		if (Wave::data == id)
		{
			break;
		}

	}

	// �f�[�^�̃T�C�Y��ǂݎ��
	file_.read(reinterpret_cast<char*>(&dataSize_), sizeof(dataSize_));

	// �\�[�X�{�C�X�̍쐬
	device->CreateSourceVoice(&source_, &format.Format, 0, 2.0f, collback_.get());
	bytesPerSec = format.Format.nAvgBytesPerSec;
	
	nowSize_ = 0u;
	bufferData_.resize(std::min(bytesPerSec, dataSize_));
	streamData_.resize(std::min(bytesPerSec, dataSize_));
	
	// �f�[�^�ǂݍ���
	file_.read(reinterpret_cast<char*>(bufferData_.data()), std::min(bytesPerSec, dataSize_));

	// �o�b�t�@�[�ɃZ�b�g����
	buffer_ = std::make_unique<XAUDIO2_BUFFER>();
	buffer_->AudioBytes = std::min(bytesPerSec, dataSize_);
	buffer_->pAudioData = bufferData_.data();
	buffer_->LoopCount = 0;
	buffer_->Flags = XAUDIO2_END_OF_STREAM;
	source_->SubmitSourceBuffer(buffer_.get());

	

	streamThread_ = std::thread{ &Xa2SoundStreamSpeaker::Worker,this };
}

Eugene::Xa2SoundStreamSpeaker::~Xa2SoundStreamSpeaker()
{
	isRun_.store(false);
	streamThread_.join();
	source_->DestroyVoice();
}

void Eugene::Xa2SoundStreamSpeaker::Play(void)
{
	source_->Start();
	isPlay_.store(true);
}

void Eugene::Xa2SoundStreamSpeaker::Stop(void)
{
	isPlay_.store(false);
	source_->Stop();
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
	XAUDIO2_VOICE_STATE state;
	while (isRun_.load())
	{
		source_->GetState(&state);

		// �X�e�[�g���`�F�b�N
		if (state.BuffersQueued <= 0 && isPlay_.load())
		{
			// �o�b�t�@�[�̍Đ����I�����Đ����ׂ���

			auto nextSize = std::min(dataSize_ - nowSize_, bytesPerSec);
			if (nextSize <= 0u)
			{
				// �Đ����ׂ��T�C�Y��0�̎����[�v�𔲂���
				break;
			}

			// ���炩���ߓǂݍ��񂾃f�[�^�Ɠ���ւ���
			bufferData_.swap(streamData_);

			// �o�b�t�@�[���Z�b�g
			buffer_->AudioBytes = streamSize_;
			buffer_->pAudioData = bufferData_.data();
			buffer_->LoopCount = 0;
			buffer_->Flags = XAUDIO2_END_OF_STREAM;

			// �Đ�
			source_->SubmitSourceBuffer(buffer_.get());
			source_->Start();

			// �ǂݍ���
			file_.read(reinterpret_cast<char*>(streamData_.data()), nextSize);
			streamSize_ = nextSize;
			nowSize_ += streamSize_;
		}
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
