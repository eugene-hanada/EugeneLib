﻿#include "../../../Include/Sound/Xaudio2/Xa2SoundStreamSpeaker.h"
#include "../../../Include/Sound/SoundCommon.h"
#include "../../../Include/Sound/SoundControl.h"
#include "../../../Include/Sound/SoundFile.h"
#include "../../../Include/Debug/Debug.h"

#include "../../../Include/Sound/SoundStreamFile.h"

Eugene::SoundStreamSpeaker::SoundStreamSpeaker(IXAudio2* xaudio2, std::unique_ptr<SoundStreamFile>&& streamFile, std::uint16_t outChannel, const float maxPitchRate):
	streamFile_{std::move(streamFile)}
{
	isPlay_.store(false);
	isRun_.store(true);
	callback_ = std::make_unique<CollBack>(*this);

	const auto& format{ streamFile_->GetFormat() };
	const auto ext{ streamFile_->GetFormatEx() };
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

	// ソースボイスの作成
	xaudio2->CreateSourceVoice(std::out_ptr(source_), &formatEx.Format, 0, maxPitchRate, callback_.get());
	buffer_ = std::make_unique<XAUDIO2_BUFFER>();

	// バイト数入れとく
	bytesPerSec = formatEx.Format.nAvgBytesPerSec;
	
	// バイト数に合わせてバッファ用データ配列をリサイズ
	bufferData_.resize(bytesPerSec);
	streamData_.resize(bytesPerSec);
	inChannel_ = formatEx.Format.nChannels;
	streamThread_ = std::thread{ &SoundStreamSpeaker::Worker,this };
}

Eugene::SoundStreamSpeaker::~SoundStreamSpeaker()
{
	isRun_.store(false);
	semaphore_.release();
	if (streamThread_.joinable())
	{
		streamThread_.join();
	}
	if (source_)
	{
		source_->Stop();
	}
}

void Eugene::SoundStreamSpeaker::Play(int loopCount)
{
	isPlay_.store(false);

	source_->Stop();
	source_->FlushSourceBuffers();
	{
		std::lock_guard<std::mutex> lock{mutex_};
		maxLoop_ = loopCount;
		nowLoop_ = 0u;
		std::fill(bufferData_.begin(), bufferData_.end(), 0);
		std::fill(streamData_.begin(), streamData_.end(), 0);
		streamFile_->SeekStart();
		SetUp();
	}
	source_->Start();
	isPlay_.store(true);
}

void Eugene::SoundStreamSpeaker::Stop(void)
{
	isPlay_.store(false);
	source_->Stop(XAUDIO2_PLAY_TAILS);
}

bool Eugene::SoundStreamSpeaker::IsEnd(void) const
{
	return isPlay_.load();
}

void Eugene::SoundStreamSpeaker::SetPitchRate(float rate)
{
	source_->SetFrequencyRatio(rate);
}

void Eugene::SoundStreamSpeaker::SetOutput(SoundControl& control)
{
	outChannel_ = control.GetInChannel();
	auto ptr{ static_cast<IXAudio2SubmixVoice*>(control.Get()) };
	XAUDIO2_SEND_DESCRIPTOR sDescriptor{ 0,ptr };
	XAUDIO2_VOICE_SENDS sends{ 1, &sDescriptor };
	source_->SetOutputVoices(&sends);
}

void Eugene::SoundStreamSpeaker::SetVolume(float volume)
{
	if (volume != volume_)
	{
		volume_ = volume;
		source_->SetVolume(volume * volume);
	}
}

void Eugene::SoundStreamSpeaker::SetPan(std::span<float> volumes)
{
	if ((inChannel_ * outChannel_) >= volumes.size())
	{
		source_->SetOutputMatrix(nullptr, inChannel_, outChannel_, volumes.data());
	}
}

void Eugene::SoundStreamSpeaker::SetUp(void)
{
	// 読み込むデータのサイズ
	auto size{ std::min(bytesPerSec, streamFile_->GetDataSize()) };

	// データ読み込み
	streamFile_->Read(bufferData_.data(), size);

	// バッファーにセットする
	buffer_->AudioBytes = size;
	buffer_->pAudioData = bufferData_.data();
	buffer_->LoopCount = 0;
	buffer_->Flags = XAUDIO2_END_OF_STREAM;
	source_->SubmitSourceBuffer(buffer_.get());

	streamSize_ = 0u;
	if ((streamFile_->GetDataSize() - streamFile_->GetReadSize()) > 0u)
	{
		// データがまだある場合

		// 1秒分もしくは残りのデータのサイズを求める
		streamSize_ = std::min(bytesPerSec, (streamFile_->GetDataSize() - streamFile_->GetReadSize()));

		// 読み込む
		streamFile_->Read(streamData_.data(), streamSize_);
	}
}

void Eugene::SoundStreamSpeaker::Worker(void)
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
			auto nextSize = std::min(streamFile_->GetDataSize() - streamFile_->GetReadSize(), bytesPerSec);

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

				streamSize_ = nextSize;
				streamFile_->Read(streamData_.data(), nextSize);

				if (streamFile_->GetReadSize() >= streamFile_->GetDataSize())
				{
					if (++nowLoop_ > maxLoop_ && maxLoop_ >= 0)
					{
						// 最大ループ回数に達したもしくは無限ループではないとき終了する
						isPlay_.store(false);
					}
					else
					{
						// データもしくはファイル末端まで読み込んだ時
						streamFile_->SeekStart();
					}
				}

			}
		}
	}
}


Eugene::SoundStreamSpeaker::CollBack::CollBack(SoundStreamSpeaker& speaker) :
	speaker_{&speaker}
{
}

void Eugene::SoundStreamSpeaker::CollBack::OnBufferEnd(void* pBufferContext) noexcept
{
	// 待機を解除
	speaker_->semaphore_.release();
}

void Eugene::SoundStreamSpeaker::CollBack::OnBufferStart(void* pBufferContext) noexcept
{
}

void Eugene::SoundStreamSpeaker::CollBack::OnLoopEnd(void* pBufferContext) noexcept
{
}

void Eugene::SoundStreamSpeaker::CollBack::OnStreamEnd() noexcept
{
}

void Eugene::SoundStreamSpeaker::CollBack::OnVoiceError(void* pBufferContext, HRESULT Error) noexcept
{
}

void Eugene::SoundStreamSpeaker::CollBack::OnVoiceProcessingPassEnd() noexcept
{
}

void Eugene::SoundStreamSpeaker::CollBack::OnVoiceProcessingPassStart(std::uint32_t BytesRequired) noexcept
{
}

Eugene::SoundStreamSpeaker::SoundStreamSpeaker(SoundStreamSpeaker&& streamSpeaker) noexcept
	:
	SoundBase{ std::move(streamSpeaker) },
	source_{ std::move(streamSpeaker.source_) }, callback_{ std::move(streamSpeaker.callback_) }, streamThread_{ std::move(streamSpeaker.streamThread_) },
	isRun_{ streamSpeaker.isRun_.load() }, isPlay_{ streamSpeaker.isPlay_.load() }, semaphore_{ 0 },
	streamFile_{ std::move(streamSpeaker.streamFile_) }, buffer_{ std::move(streamSpeaker.buffer_) }, bufferData_{ std::move(streamSpeaker.bufferData_) }, streamData_{ std::move(streamSpeaker.streamData_) },
	streamSize_{ streamSpeaker.streamSize_ }, bytesPerSec{ streamSpeaker.bytesPerSec }, nowLoop_{ streamSpeaker.nowLoop_ }, maxLoop_{ streamSpeaker.maxLoop_ }
{
	streamThread_.detach();
	callback_->SetStreamSpeaker(*this);
	streamThread_ = std::thread{ &SoundStreamSpeaker::Worker,this };
}

Eugene::SoundStreamSpeaker& Eugene::SoundStreamSpeaker::operator=(SoundStreamSpeaker&& streamSpeaker) noexcept
{
	static_cast<SoundBase&>(*this) = std::move(streamSpeaker);
	source_ = std::move(streamSpeaker.source_);
	callback_ = std::move(streamSpeaker.callback_);
	streamThread_ = std::move(streamSpeaker.streamThread_);
	streamThread_.detach();
	callback_->SetStreamSpeaker(*this);
	streamThread_ = std::thread{ &SoundStreamSpeaker::Worker,this };
	
	isRun_.store(streamSpeaker.isRun_.load());
	isPlay_.store(streamSpeaker.isPlay_.load());
	streamFile_ = std::move(streamSpeaker.streamFile_);
	buffer_ = std::move(streamSpeaker.buffer_);
	bufferData_ = std::move(streamSpeaker.bufferData_);
	streamData_ = std::move(streamSpeaker.streamData_);
	streamSize_ = streamSpeaker.streamSize_;
	bytesPerSec = streamSpeaker.bytesPerSec;
	nowLoop_ = streamSpeaker.nowLoop_;
	maxLoop_ = streamSpeaker.maxLoop_;
	return *this;
}