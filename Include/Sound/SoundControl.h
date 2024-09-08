#pragma once
#ifdef EUGENE_XAUDIO
#include "Xaudio2/Xa2SoundControl.h"
#elif EUGENE_ANDROID
#include "AAudio/AaSoundControl.h"
#endif

//namespace Eugene
//{
//	/// <summary>
//	/// サウンドの出力をコントロールするクラス(音量、パンなどに使用)
//	/// </summary>
//	class SoundControl :
//		public SoundBase
//	{
//	public:
//		virtual void* Get(void) = 0;
//		virtual void SetVolume(float volume) = 0;
//		virtual void SetPan(std::span<float> volumes) = 0;
//		virtual void SetOutput(SoundControl& control) = 0;
//	protected:
//		SoundControl(std::uint32_t sample, std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t stage);
//		friend class Sound;
//	};
//
//}