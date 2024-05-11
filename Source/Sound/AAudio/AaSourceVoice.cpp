//
// Created by eugen on 2024/04/27.
//

#include "AaSourceVoice.h"
#include "../../../Include/Sound/SoundBase.h"

void AaSourceVoice::GetNextFrame(std::span<float> outSpan) {
    auto size = dataSize_ - playBytye_;
    if (size <= 0)
    {

        if (loopCount_ == 0)
        {
            if (callBack_ != nullptr && !isEnd_)
            {
                callBack_->OnPlayEnd(loopCount_);
            }
            isEnd_ = true;
            return;
        }

        if (loopCount_ != -1)
        {
            loopCount_--;
        }
        playBytye_ = 0ull;
    }

    (this->*getNextFrameFunc_)(outSpan,size);

    playBytye_ += byteParFrame_;
}

AaSourceVoice::AaSourceVoice(Eugene::SoundBase &soundBase, AaSubmix *submix,
                             const Eugene::SoundFormat& format,CallBack* callBack ) :
        AaVoice{soundBase,submix,format.sample},
        callBack_{callBack},loopCount_{0},dataSize_{0},playBytye_{0},isEnd_{true}
{
    auto sample = format.sample;
    byteParFrame_ = (sample * (soundBase_.GetInChannel() * format.bit) / 8ull) / sample;

    getNextFrameFunc_ = &AaSourceVoice::GetPcm16NextFrame;
    if (format.bit == 32)
    {
        getNextFrameFunc_ = &AaSourceVoice::GetPcm32NextFrame;
    }
}

void AaSourceVoice::GetPcm16NextFrame(std::span<float> outSpan, std::uint64_t size) {
    std::span<std::int16_t> now{
            reinterpret_cast<std::int16_t*>(dataPtr_ + playBytye_),
            static_cast<std::size_t>(size /2ull )
    };
    for (int y = 0; y < soundBase_.GetOutChannel(); y++)
    {
        for (int x = 0; x < soundBase_.GetInChannel(); x++)
        {
            outSpan[y] += (static_cast<float>(now[x]) /
                           static_cast<float>(std::numeric_limits<std::int16_t >::max())) *
                          outMatrix_[(y *  soundBase_.GetOutChannel()) + x] *
                          soundBase_.GetVolume();
        }
    }
}

void AaSourceVoice::GetPcm32NextFrame(std::span<float> outSpan, std::uint64_t size) {
    std::span<std::int32_t> now{
            reinterpret_cast<std::int32_t *>(dataPtr_ + playBytye_),
            static_cast<std::size_t>(size /4ull )
    };
    for (int y = 0; y < soundBase_.GetOutChannel(); y++)
    {
        for (int x = 0; x < soundBase_.GetInChannel(); x++)
        {
            outSpan[y] += (static_cast<float>(now[x]) /
                           static_cast<float>(std::numeric_limits<std::int32_t >::max())) *
                          outMatrix_[(y * soundBase_.GetOutChannel()) + x] * soundBase_.GetVolume();
        }
    }
}

void AaSourceVoice::SetData(const std::uint8_t *ptr, const std::uint64_t size) {
    dataPtr_ = (std::uint8_t*)ptr;
    dataSize_ = size;
    playBytye_ = dataSize_;
}

void AaSourceVoice::Play(std::int32_t loopCount) {
    loopCount_ = loopCount;
    playBytye_ = 0;
    isEnd_ = false;
}

void AaSourceVoice::Stop() {
    playBytye_ = dataSize_;
}

bool AaSourceVoice::IsEnd() const{
    if ((dataSize_ - playBytye_) <= 0u)
    {
        return loopCount_ == 0u;
    }
    return false;
}

void AaSourceVoice::SetFormat(const Eugene::SoundFormat format) {
    SetChannel(soundBase_.GetInChannel(),soundBase_.GetOutChannel());
    auto sample = format.sample;
    byteParFrame_ = (sample * (soundBase_.GetInChannel() * format.bit) / 8ull) / sample;

    getNextFrameFunc_ = &AaSourceVoice::GetPcm16NextFrame;
    if (format.bit == 32)
    {
        getNextFrameFunc_ = &AaSourceVoice::GetPcm32NextFrame;
    }
}
