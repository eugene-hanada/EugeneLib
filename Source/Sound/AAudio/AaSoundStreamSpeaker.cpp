#include "../../../Include/Sound/AAudio/AaSoundStreamSpeaker.h"
#include "../../../Include/Sound/SoundStreamFile.h"
#include "../../../Include/Sound/SoundControl.h"

#include "../../../Include/Debug/Debug.h"

Eugene::SoundStreamSpeaker::SoundStreamSpeaker(std::unique_ptr<SoundStreamFile>&& streamFile,AaSubmix*  submix,std::uint16_t outChannel) :
        SoundBase{streamFile->GetFormat().channel, outChannel},
        callBack_{*this},
        voice_{static_cast<SoundBase&>(*this),submix,streamFile->GetFormat(), &callBack_},
        streamFile_{std::move(streamFile)},
        nowLoop_ {0}, maxLoop_{ 0 }, streamSize_{0}
{
    auto bytePerSec{streamFile_->GetFormat().byte};
    bufferData_.resize(bytePerSec);
    streamData_.resize(bytePerSec);
    isRun_.store(true);
    isPlay_.store(false);
    streamThread_ = std::thread{ &SoundStreamSpeaker::Worker,this };
}

Eugene::SoundStreamSpeaker::~SoundStreamSpeaker()
{
    isRun_.store(false);
    streamThread_.join();
}

void Eugene::SoundStreamSpeaker::Play(int loopCount)
{
    isPlay_.store(false);
    voice_.Stop();
    {
        std::lock_guard<std::mutex> lock{mutex_};
        maxLoop_ = loopCount;
        nowLoop_ = 0u;
        std::fill(bufferData_.begin(), bufferData_.end(), 0);
        std::fill(streamData_.begin(), streamData_.end(), 0);
        streamFile_->SeekStart();
        SetUp();
    }
    voice_.Play(0);
    isPlay_.store(true);
}

void Eugene::SoundStreamSpeaker::Stop(void)
{
    isPlay_.store(false);
    voice_.Stop();
}

bool Eugene::SoundStreamSpeaker::IsEnd(void) const
{
	return true;
}

void Eugene::SoundStreamSpeaker::SetPitchRate(float rate)
{
	
}

void Eugene::SoundStreamSpeaker::SetOutput(SoundControl& control)
{
    outChannel_ = control.GetInChannel();
    auto submix = reinterpret_cast<AaSubmix*>(control.Get());
    voice_.SetOut(submix, inChannel_,outChannel_);
}

void Eugene::SoundStreamSpeaker::SetVolume(float volume)
{
    volume_ = volume * volume;
}

void Eugene::SoundStreamSpeaker::SetPan(std::span<float> volumes)
{
    voice_.SetOutMatrix(volumes);
}

void Eugene::SoundStreamSpeaker::SetUp(void) {
    // 読み込むデータのサイズ
    auto size{
        std::min(static_cast<std::uint64_t>(streamFile_->GetFormat().byte),
                 streamFile_->GetDataSize())
    };

    // データ読み込み
    streamFile_->Read(bufferData_.data(), size);

    // バッファーにセットする
    voice_.SetData(bufferData_.data(),size);

    streamSize_ = 0u;
    if ((streamFile_->GetDataSize() - streamFile_->GetReadSize()) > 0u)
    {
        // データがまだある場合

        // 1秒分もしくは残りのデータのサイズを求める
        streamSize_ = std::min(static_cast<std::uint64_t>(streamFile_->GetFormat().byte), (streamFile_->GetDataSize() - streamFile_->GetReadSize()));

        // 読み込む
        streamFile_->Read(streamData_.data(), streamSize_);
    }
}

void Eugene::SoundStreamSpeaker::Worker(void) {
    while (true)
    {
        // 待機
        semaphore_.acquire();

        if (!isRun_.load())
        {
            //DebugClass.Log("Break");
            // 実行フラグがfalseの時ループを抜け処理をやめる
            break;
        }

        if (isPlay_.load())
        {
            auto nextSize = std::min(
                    streamFile_->GetDataSize() - streamFile_->GetReadSize(),
                    static_cast<std::uint64_t>(streamFile_->GetFormat().byte)
                    );

            if (nextSize > 0)
            {
                std::lock_guard<std::mutex> lock{mutex_};

                // あらかじめ読み込んだデータと入れ替える
                bufferData_.swap(streamData_);

                //DebugClass.Log("SetData size={}", streamSize_);
                // バッファーをセット
                voice_.SetData(bufferData_.data(),streamSize_);
                voice_.Play(0);
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

Eugene::SoundStreamSpeaker::SoundStreamSpeaker(
        Eugene::SoundStreamSpeaker &&streamSpeaker) noexcept :
        SoundBase{std::move(streamSpeaker)}, voice_{std::move(streamSpeaker.voice_)},
        callBack_{std::move(streamSpeaker.callBack_)}, streamFile_{std::move(streamSpeaker.streamFile_)},
        streamData_{std::move(streamSpeaker.streamData_)}, bufferData_{std::move(streamSpeaker.bufferData_)},
        streamThread_{std::move(streamSpeaker.streamThread_)},semaphore_{0},
        streamSize_{streamSpeaker.streamSize_}, nowLoop_{streamSpeaker.nowLoop_}, maxLoop_{streamSpeaker.maxLoop_}
{
    streamThread_.detach();
    callBack_.Set(this);
    streamThread_ = std::thread{ &SoundStreamSpeaker::Worker,this };
}

Eugene::SoundStreamSpeaker &
Eugene::SoundStreamSpeaker::operator=(Eugene::SoundStreamSpeaker &&streamSpeaker) noexcept
{
    static_cast<SoundBase&>(*this) = std::move(streamSpeaker);
    voice_ = std::move(streamSpeaker.voice_);
    callBack_ = std::move(streamSpeaker.callBack_);
    streamThread_ = std::move(streamSpeaker.streamThread_);
    streamThread_.detach();
    callBack_.Set(this);
    streamThread_ = std::thread{ &SoundStreamSpeaker::Worker,this };

    isRun_.store(streamSpeaker.isRun_.load());
    isPlay_.store(streamSpeaker.isPlay_.load());
    streamFile_ = std::move(streamSpeaker.streamFile_);
    bufferData_ = std::move(streamSpeaker.bufferData_);
    streamData_ = std::move(streamSpeaker.streamData_);
    streamSize_ = streamSpeaker.streamSize_;
    nowLoop_ = streamSpeaker.nowLoop_;
    maxLoop_ = streamSpeaker.maxLoop_;
    return *this;
}


void Eugene::SoundStreamSpeaker::StreamCallBack::OnPlayEnd(std::int32_t loopCount)
{
    // 待機を解除
    streamSpeaker_->semaphore_.release();
}

Eugene::SoundStreamSpeaker::StreamCallBack::StreamCallBack(
        Eugene::SoundStreamSpeaker &streamSpeaker) :
        streamSpeaker_{&streamSpeaker}
{

}
