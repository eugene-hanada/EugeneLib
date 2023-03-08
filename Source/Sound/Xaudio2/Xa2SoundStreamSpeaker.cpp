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

	// fmtチャンクを見つける
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
	
	// データチャンクを見つける
	while (true)
	{
		file_.read(reinterpret_cast<char*>(&id), 4);
		if (Wave::data == id)
		{
			break;
		}

	}

	// データのサイズを読み取る
	file_.read(reinterpret_cast<char*>(&dataSize_), sizeof(dataSize_));

	// ソースボイスの作成
	device->CreateSourceVoice(&source_, &format.Format, 0, 2.0f, collback_.get());
	bytesPerSec = format.Format.nAvgBytesPerSec;
	
	
	bufferData_.resize(bytesPerSec);
	streamData_.resize(bytesPerSec);
	
	// 読み込むデータのサイズ
	auto size = std::min(bytesPerSec, dataSize_);

	// データ読み込み
	file_.read(reinterpret_cast<char*>(bufferData_.data()), size);

	// バッファーにセットする
	buffer_ = std::make_unique<XAUDIO2_BUFFER>();
	buffer_->AudioBytes = size;
	buffer_->pAudioData = bufferData_.data();
	buffer_->LoopCount = 0;
	buffer_->Flags = XAUDIO2_END_OF_STREAM;
	nowSize_ = size;
	source_->SubmitSourceBuffer(buffer_.get());
	
	streamSize_ = 0u;
	if ((dataSize_ - nowSize_) > 0u)
	{
		// データがまだある場合

		// 1秒分もしくは残りのデータのサイズを求める
		streamSize_ = std::min(bytesPerSec,(dataSize_ - nowSize_));

		// 読み込む
		file_.read(reinterpret_cast<char*>(bufferData_.data()), streamSize_);
	}
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

		// ステートをチェック
		if (state.BuffersQueued <= 0 && isPlay_.load())
		{
			// バッファーの再生が終了かつ再生すべき時

			auto nextSize = std::min(dataSize_ - nowSize_, bytesPerSec);
			if (nextSize <= 0u)
			{
				// 再生すべきサイズが0の時ループを抜ける
				break;
			}

			// あらかじめ読み込んだデータと入れ替える
			bufferData_.swap(streamData_);

			// バッファーをセット
			buffer_->AudioBytes = streamSize_;
			buffer_->pAudioData = bufferData_.data();
			buffer_->LoopCount = 0;
			buffer_->Flags = XAUDIO2_END_OF_STREAM;

			// 再生
			source_->SubmitSourceBuffer(buffer_.get());
			source_->Start();

			// 読み込む
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
