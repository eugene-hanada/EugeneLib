#include "Xa2SoundStreamSpeaker.h"
#include "../../../Include/Sound/SoundCommon.h"
#include "../Wave.h"
#include "../../../Include/Sound/SoundControl.h"

#include "../../../Include/Common/Debug.h"


Eugene::Xa2SoundStreamSpeaker::Xa2SoundStreamSpeaker(IXAudio2* device, const std::filesystem::path& path, std::uint16_t outChannel, const float maxPitchRate) :
	SoundStreamSpeaker{ maxPitchRate }, nowLoop_{0}, maxLoop_{0}
{
	outChannel_ = outChannel;
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
	inChannel_ = format.Format.nChannels;
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
	
	// データの先頭の位置を保持
	starPos_ = file_.tellg();

	// ソースボイスの作成
	device->CreateSourceVoice(&source_, &format.Format, 0, 2.0f, collback_.get());
	buffer_ = std::make_unique<XAUDIO2_BUFFER>();

	// バイト数入れとく
	bytesPerSec = format.Format.nAvgBytesPerSec;
	
	// バイト数に合わせてバッファ用データ配列をリサイズ
	bufferData_.resize(bytesPerSec);
	streamData_.resize(bytesPerSec);
	
	SetUp();

	streamThread_ = std::thread{ &Xa2SoundStreamSpeaker::Worker,this };
}

Eugene::Xa2SoundStreamSpeaker::~Xa2SoundStreamSpeaker()
{
	isRun_.store(false);
	semaphore_.release();
	streamThread_.join();
	source_->Stop();
	source_->DestroyVoice();
}

void Eugene::Xa2SoundStreamSpeaker::Play(int loopCount)
{
	maxLoop_ = loopCount;
	nowLoop_ = 0u;
	source_->Start();
	isPlay_.store(true);
}

void Eugene::Xa2SoundStreamSpeaker::Stop(void)
{
	isPlay_.store(false);
	source_->Stop(XAUDIO2_PLAY_TAILS);
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
	outChannel_ = control.GetInChannel();
	auto ptr = static_cast<IXAudio2SubmixVoice*>(control.Get());
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	source_->SetOutputVoices(&sends);
}

void Eugene::Xa2SoundStreamSpeaker::SetVolume(float volume)
{
	auto vol = volume * volume;
	if (vol != volume_)
	{
		source_->SetVolume(vol);
	}
}

void Eugene::Xa2SoundStreamSpeaker::SetPan(std::span<float> volumes)
{
	if ((inChannel_ * outChannel_) + inChannel_ >= volumes.size())
	{
		source_->SetOutputMatrix(nullptr, inChannel_, outChannel_, volumes.data());
	}
}

void Eugene::Xa2SoundStreamSpeaker::SetUp(void)
{
	file_.seekg(starPos_);

	// 読み込むデータのサイズ
	auto size = std::min(bytesPerSec, dataSize_);

	// データ読み込み
	file_.read(reinterpret_cast<char*>(bufferData_.data()), size);

	// バッファーにセットする
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
		streamSize_ = std::min(bytesPerSec, (dataSize_ - nowSize_));

		// 読み込む
		file_.read(reinterpret_cast<char*>(streamData_.data()), streamSize_);
	}
}

void Eugene::Xa2SoundStreamSpeaker::Worker(void)
{
	while (true)
	{
		// 待機
		semaphore_.acquire();

		if (!isRun_.load())
		{
			break;
		}

		if (isPlay_.load())
		{
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
			//DebugLog("再生");
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
	// 待機を解除
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
