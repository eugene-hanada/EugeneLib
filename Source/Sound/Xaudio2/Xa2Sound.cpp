#include "Xa2Sound.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include "../../../Include/Common/EugeneLibException.h"
#include "Xa2Sound3DControl.h"
#include "../../../Include/Sound/SoundSpeaker.h"
#include "Xa2SoundControl.h"
#include "../../../Include/Sound/SoundStreamSpeaker.h"

#pragma comment (lib,"xaudio2.lib")

X3DAUDIO_HANDLE handle;

Eugene::Sound::SoundImpl::SoundImpl(Sound& sound):
	sound_{sound}
{
	if (FAILED(XAudio2Create(&xaudio2_, 0)))
	{
		throw CreateErrorException("XAudio2生成失敗");
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
		throw CreateErrorException("マスタリングボイスの作成に失敗");
	}
	
	XAUDIO2_VOICE_DETAILS details;
	mastering_->GetVoiceDetails(&details);
	sound_.inChannel_ = sound_.outChannel_ = details.InputChannels;
	sound_.sampleRate_ = details.InputSampleRate;
	DWORD tmpMask;
	mastering_->GetChannelMask(&tmpMask);
	sound_.channelMask_ = tmpMask;
	
	if (FAILED(X3DAudioInitialize(sound_.channelMask_, 340.0f, handle)))
	{
		throw CreateErrorException("X3DAudioの初期化に失敗");
	}
}

Eugene::Sound::SoundImpl::~SoundImpl()
{
	mastering_->DestroyVoice();
}

void Eugene::Sound::SoundImpl::SetVolume(float volume)
{
	if (sound_.volume_ != volume)
	{
		sound_.volume_ = volume;
		mastering_->SetVolume(volume * volume);
	}
}

void Eugene::Sound::SoundImpl::SetPan(std::span<float> volumes)
{
	if (sound_.outChannel_ == volumes.size())
	{
		mastering_->SetOutputMatrix(nullptr, sound_.outChannel_, sound_.outChannel_, volumes.data());
	}
}

Eugene::SoundSpeaker* Eugene::Sound::SoundImpl::CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate) const
{
	return new SoundSpeaker{reinterpret_cast<std::uintptr_t>(xaudio2_.Get()),soundFile, sound_.outChannel_, maxPitchRate};
}


Eugene::SoundStreamSpeaker* Eugene::Sound::SoundImpl::CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const
{
	return new SoundStreamSpeaker{reinterpret_cast<std::uintptr_t>(xaudio2_.Get()), path,sound_.outChannel_,maxPitchRate};
}

Eugene::SoundControl* Eugene::Sound::SoundImpl::CreateSoundControl(std::uint32_t stage, std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel) const
{
	return new Xa2SoundControl{
		xaudio2_.Get(),
		(sample == 0u ? sound_.sampleRate_ : sample),
		(inputChannel == 0u ? sound_.inChannel_ : inputChannel),
		(outChannel == 0u ? sound_.inChannel_ : outChannel) ,
		stage
	};
}

Eugene::Sound3DControl* Eugene::Sound::SoundImpl::CreateSound3DControl(std::uint32_t stage, std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel) const
{
	return new Xa2Sound3DControl{
		xaudio2_.Get(), 
		handle,
		(outChannel == 0u ? sound_.inChannel_ : outChannel),
		(inputChannel == 0u ? sound_.inChannel_ : inputChannel),
		(sample == 0u ? sound_.sampleRate_ : sample),
		stage
	};
}

void* Eugene::Sound::SoundImpl::GetDevice(void)
{
	return xaudio2_.Get();
}
