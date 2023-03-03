﻿#include "Xa2Sound.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include "../../../Include/Common/EugeneLibException.h"
#include "Xa2Sound3DControl.h"
#include "Xa2SoundSpeaker.h"
#include "Xa2SoundControl.h"
#include "Xa2SoundStreamSpeaker.h"

#pragma comment (lib,"xaudio2.lib")

X3DAUDIO_HANDLE handle;

Eugene::Xa2Sound::Xa2Sound()
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
	
	XAUDIO2_VOICE_DETAILS details;
	mastering_->GetVoiceDetails(&details);
	inChannel_ = outChannel_ = details.InputChannels;

	DWORD tmpMask;
	mastering_->GetChannelMask(&tmpMask);
	channelMask_ = tmpMask;
	
	if (FAILED(X3DAudioInitialize(channelMask_, 340.0f, handle)))
	{
		throw EugeneLibException("X3DAudioの初期化に失敗");
	}
}

Eugene::Xa2Sound::~Xa2Sound()
{
	mastering_->DestroyVoice();
}

void Eugene::Xa2Sound::SetVolume(float volume)
{
	mastering_->SetVolume(volume * volume);
}

void Eugene::Xa2Sound::SetPan(std::span<float> volumes)
{
	if (outChannel_ == volumes.size())
	{
		mastering_->SetOutputMatrix(nullptr, outChannel_, outChannel_, volumes.data());
	}
}

Eugene::SoundSpeaker* Eugene::Xa2Sound::CreateSoundSpeaker(const Wave& wave, const float maxPitchRate ) const
{
	return new Xa2SoundSpeaker{xaudio2_.Get(),wave, inChannel_, maxPitchRate};
}

Eugene::SoundStreamSpeaker* Eugene::Xa2Sound::CreateSoundStreamSpeaker(const std::filesystem::path& path) const
{
	return new Xa2SoundStreamSpeaker{xaudio2_.Get(), path, outChannel_};
}

Eugene::SoundControl* Eugene::Xa2Sound::CreateSoundControl(std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel) const
{
	return new Xa2SoundControl{xaudio2_.Get(), sample, inputChannel, outChannel};
}

Eugene::Sound3DControl* Eugene::Xa2Sound::CreateSound3DControl(std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel) const
{
	return new Xa2Sound3DControl{ xaudio2_.Get(), handle, outChannel, inputChannel, sample };
}
