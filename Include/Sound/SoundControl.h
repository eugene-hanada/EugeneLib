#pragma once
#include "SoundBase.h"
#include <memory>

namespace Eugene
{
	/// <summary>
	/// サウンドの出力をコントロールするクラス(音量、パンなどに使用)
	/// </summary>
	class SoundControl :
		public SoundBase
	{
	public:
		void* Get(void);
		void SetVolume(float volume) final;
		void SetPan(std::span<float> volumes) final;
		void SetOutput(SoundControl& control) final;
	protected:
        class SoundControlImpl;

        struct SoundControlImplDeleter
        {
            void operator()(SoundControlImpl* ptr);
        };

		SoundControl(std::uintptr_t devicePtr, std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t stage);
		std::unique_ptr<SoundControlImpl, SoundControlImplDeleter> impl_;
		friend class Sound;
	};

}