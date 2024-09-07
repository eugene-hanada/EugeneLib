#include "../../../Include/Sound/Xaudio2/Xa2Sound.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include "../../../Include/Utils/EugeneLibException.h"
#include "../../../Include/Sound/Sound3DControl.h"

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

//Eugene::SoundSpeaker* Eugene::Sound::CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate) const
//{
//	return new Xaudio2Speaker{xaudio2_.Get(),soundFile,outChannel_,maxPitchRate};
//}
//
//
//Eugene::SoundStreamSpeaker* Eugene::Xaudio2Sound::CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const
//{
//	return new Xaudio2StreamSpeaker{xaudio2_.Get(), CreateSoundStreamFile(path),outChannel_,maxPitchRate};
//}
//
//Eugene::SoundControl* Eugene::Xaudio2Sound::CreateSoundControl(std::uint32_t stage, std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel) const
//{
//	return new Xaudio2Control{
//		xaudio2_.Get(),
//		(sample == 0u ? sampleRate_ : sample),
//		stage,
//		(inputChannel == 0u ? inChannel_ : inputChannel),
//		(outChannel == 0u ? inChannel_ : outChannel) 
//		
//	};
//}
//
//Eugene::Sound3DControl* Eugene::Xaudio2Sound::CreateSound3DControl(std::uint32_t stage, std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel) const
//{
//	std::span<std::uint8_t, 20> h{ handle };
//	return new Xaudio23DControl{
//		xaudio2_.Get(), 
//		(sample == 0u ? sampleRate_ : sample),
//		stage,
//		(outChannel == 0u ?inChannel_ : outChannel),
//		(inputChannel == 0u ? inChannel_ : inputChannel),
//		h
//	};
//}

