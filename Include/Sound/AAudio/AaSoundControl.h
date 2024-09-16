#pragma once
#include <memory>
#include "AaVoice.h"
#include "AaSubmix.h"
#include "../SoundBase.h"

namespace Eugene
{

    /// <summary>
    /// サウンドコントロールクラス
    /// </summary>
    class SoundControl :
            public SoundBase
    {
    public:
        ~SoundControl();
        void SetPan(std::span<float> volumes) final;
        void SetVolume(float volume) final;
        void SetOutput(SoundControl& control) final;
        void* Get(void) noexcept
        {
            return &submix_;
        }
        void SetOutChannel(std::uint16_t channel);

        SoundControl(SoundControl&& control) noexcept :
                SoundBase{std::move(control)}, submix_{std::move(control.submix_)}
        {

        }

        SoundControl& operator=(SoundControl&& control) noexcept
        {
            static_cast<SoundBase&>(*this) = std::move(control);
            submix_ = std::move(control.submix_);
            return *this;
        }

        SoundControl(const SoundControl&) = delete;
        SoundControl& operator=(const SoundControl&) = delete;

        void Final() noexcept
        {
            // 一応用意
        }
    private:
        SoundControl(AaSubmix*  submix, std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t stage);
        AaSubmix submix_;
        friend class Sound;
    };
}


