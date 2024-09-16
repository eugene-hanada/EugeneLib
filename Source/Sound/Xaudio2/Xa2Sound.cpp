#include "../../../Include/Sound/Xaudio2/Xa2Sound.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include "../../../Include/Utils/EugeneLibException.h"

#include "../../../Include/Sound/SoundControl.h"
#include "../../../Include/Sound/SoundStreamSpeaker.h"

#pragma comment (lib,"xaudio2.lib")

namespace
{
	X3DAUDIO_HANDLE handle;
}
Eugene::Sound::Sound()
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
	if (FAILED(xaudio2_->CreateMasteringVoice(std::out_ptr(mastering_))))
	{
		throw EugeneLibException("マスタリングボイスの作成に失敗");
	}
	
	XAUDIO2_VOICE_DETAILS details;
	mastering_->GetVoiceDetails(&details);
	inChannel_ = outChannel_ = details.InputChannels;
	sampleRate_ = details.InputSampleRate;
	DWORD tmpMask;
	mastering_->GetChannelMask(&tmpMask);
	channelMask_ = tmpMask;
	
	if (FAILED(X3DAudioInitialize(channelMask_, 340.0f, handle)))
	{
		throw EugeneLibException("X3DAudioの初期化に失敗");
	}
}

Eugene::Sound::~Sound()
{
}

void Eugene::Sound::SetVolume(float volume)
{
	if (volume_ != volume)
	{
		volume_ = volume;
		mastering_->SetVolume(volume * volume);
	}
}

void Eugene::Sound::SetPan(std::span<float> volumes)
{
	if (outChannel_ == volumes.size())
	{
		mastering_->SetOutputMatrix(nullptr, outChannel_, outChannel_, volumes.data());
	}
}
