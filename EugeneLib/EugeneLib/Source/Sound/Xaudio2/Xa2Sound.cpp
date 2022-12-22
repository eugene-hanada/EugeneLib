#include "Xa2Sound.h"
#include <xaudio2.h>
#include "../../../Include/Common/EugeneLibException.h"

#include "../Xaudio2/Xa2SoundSpeaker.h"

#pragma comment (lib,"xaudio2.lib")

EugeneLib::Xa2Sound::Xa2Sound()
{
	if (FAILED(XAudio2Create(&xaudio2_, 0)))
	{
		throw EugeneLibException("XAudio2生成失敗");
	}
#ifdef _DEBUG
	// デバッグ設定
	XAUDIO2_DEBUG_CONFIGURATION debug{ 0 };
	debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	debug.BreakMask = XAUDIO2_LOG_ERRORS;
	xaudio2_->SetDebugConfiguration(&debug, 0);
#endif
	if (FAILED(xaudio2_->CreateMasteringVoice(&mastering_)))
	{
		throw EugeneLibException("マスタリングボイスの作成に失敗");
	}
	mastering_->SetVolume(1.0f);
}

EugeneLib::Xa2Sound::~Xa2Sound()
{
	mastering_->DestroyVoice();
}

void EugeneLib::Xa2Sound::SetVolume(float volume)
{
	mastering_->SetVolume(volume);
}

EugeneLib::SoundSpeaker* EugeneLib::Xa2Sound::CreateSoundSpeaker(const Wave& wave) const
{
	return new Xa2SoundSpeaker{xaudio2_.Get(),wave};
}
