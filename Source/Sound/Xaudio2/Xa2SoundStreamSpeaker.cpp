﻿#include "Xa2SoundStreamSpeaker.h"
#include "../../../Include/Sound/SoundCommon.h"
#include "../../../Include/Sound/SoundControl.h"
#include "../../../Include/Sound/SoundFile.h"
#include "../../../Include/Common/Debug.h"


Eugene::Xaudio2StreamSpeaker::Xaudio2StreamSpeaker(IXAudio2* xaudio2, const std::filesystem::path& path, std::uint16_t outChannel, const float maxPitchRate) :
	SoundStreamSpeaker{outChannel,maxPitchRate}, nowLoop_ {0}, maxLoop_{ 0 }
{
	isPlay_.store(false);
	isRun_.store(true);
	collback_ = std::make_unique<CollBack>(*this);
	file_ = std::ifstream{ path, std::ios::binary };
	SoundFile::RIFF riff;
	file_.read(reinterpret_cast<char*>(&riff), sizeof(riff));
	int id = 0;

	// fmtチャンクを見つける
	while (true)
	{
		file_.read(reinterpret_cast<char*>(&id), 4);
		if (SoundFile::fmt == id)
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
		if (SoundFile::data == id)
		{
			break;
		}

	}

	// データのサイズを読み取る
	file_.read(reinterpret_cast<char*>(&dataSize_), sizeof(dataSize_));
	
	// データの先頭の位置を保持
	starPos_ = file_.tellg();

	// ソースボイスの作成
	xaudio2->CreateSourceVoice(std::out_ptr(source_), &format.Format, 0, maxPitchRate_, collback_.get());
	buffer_ = std::make_unique<XAUDIO2_BUFFER>();

	// バイト数入れとく
	bytesPerSec = format.Format.nAvgBytesPerSec;
	
	// バイト数に合わせてバッファ用データ配列をリサイズ
	bufferData_.resize(bytesPerSec);
	streamData_.resize(bytesPerSec);
	
	streamThread_ = std::thread{ &Xaudio2StreamSpeaker::Worker,this };
}

Eugene::Xaudio2StreamSpeaker::~Xaudio2StreamSpeaker()
{
	isRun_.store(false);
	semaphore_.release();
	streamThread_.join();
	source_->Stop();
	source_->DestroyVoice();
}

void Eugene::Xaudio2StreamSpeaker::Play(int loopCount)
{
	isPlay_.store(false);
	auto a = source_->Stop();
	a = source_->FlushSourceBuffers();
	{
		std::lock_guard<std::mutex> lock{mutex_};
		maxLoop_ = loopCount;
		nowLoop_ = 0u;
		std::fill(bufferData_.begin(), bufferData_.end(), 0);
		std::fill(streamData_.begin(), streamData_.end(), 0);
		file_.clear();
		file_.seekg(starPos_);
		SetUp();
	}
	source_->Start();
	isPlay_.store(true);
}

void Eugene::Xaudio2StreamSpeaker::Stop(void)
{
	isPlay_.store(false);
	source_->Stop(XAUDIO2_PLAY_TAILS);
}

bool Eugene::Xaudio2StreamSpeaker::IsEnd(void) const
{
	return isPlay_.load();
}

void Eugene::Xaudio2StreamSpeaker::SetPitchRate(float rate)
{
	source_->SetFrequencyRatio(rate);
}

void Eugene::Xaudio2StreamSpeaker::SetOutput(SoundControl& control)
{
	outChannel_ = control.GetInChannel();
	auto ptr = static_cast<IXAudio2SubmixVoice*>(control.Get());
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	source_->SetOutputVoices(&sends);
}

void Eugene::Xaudio2StreamSpeaker::SetVolume(float volume)
{
	if (volume != volume_)
	{
		volume_ = volume;
		source_->SetVolume(volume * volume);
	}
}

void Eugene::Xaudio2StreamSpeaker::SetPan(std::span<float> volumes)
{
	if ((inChannel_ * outChannel_) >= volumes.size())
	{
		source_->SetOutputMatrix(nullptr, inChannel_, outChannel_, volumes.data());
	}
}

void Eugene::Xaudio2StreamSpeaker::SetUp(void)
{
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

void Eugene::Xaudio2StreamSpeaker::Worker(void)
{
	while (true)
	{
		// 待機
		semaphore_.acquire();

		if (!isRun_.load())
		{
			// 実行フラグがfalseの時ループを抜け処理をやめる
			break;
		}

		if (isPlay_.load())
		{
			auto nextSize = std::min(dataSize_ - nowSize_, bytesPerSec);
			if (nextSize > 0)
			{
				std::lock_guard<std::mutex> lock{mutex_};

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

				file_.read(reinterpret_cast<char*>(streamData_.data()), nextSize);
				streamSize_ = nextSize;
				nowSize_ += streamSize_;

				if (nowSize_ >= dataSize_ || file_.eof())
				{
					if (++nowLoop_ > maxLoop_ && maxLoop_ >= 0)
					{
						// 最大ループ回数に達したもしくは無限ループではないとき終了する
						isPlay_.store(false);
					}
					else
					{
						// データもしくはファイル末端まで読み込んだ時
						file_.clear();
						file_.seekg(starPos_);
						nowSize_ = 0;
					}
				}

			}
		}
	}
}


Eugene::Xaudio2StreamSpeaker::CollBack::CollBack(Xaudio2StreamSpeaker& speaker) :
	speaker_{speaker}
{
}

void Eugene::Xaudio2StreamSpeaker::CollBack::OnBufferEnd(void* pBufferContext) noexcept
{
	// 待機を解除
	speaker_.semaphore_.release();
}

void Eugene::Xaudio2StreamSpeaker::CollBack::OnBufferStart(void* pBufferContext) noexcept
{
}

void Eugene::Xaudio2StreamSpeaker::CollBack::OnLoopEnd(void* pBufferContext) noexcept
{
}

void Eugene::Xaudio2StreamSpeaker::CollBack::OnStreamEnd() noexcept
{
}

void Eugene::Xaudio2StreamSpeaker::CollBack::OnVoiceError(void* pBufferContext, HRESULT Error) noexcept
{
}

void Eugene::Xaudio2StreamSpeaker::CollBack::OnVoiceProcessingPassEnd() noexcept
{
}

void Eugene::Xaudio2StreamSpeaker::CollBack::OnVoiceProcessingPassStart(std::uint32_t BytesRequired) noexcept
{
}
