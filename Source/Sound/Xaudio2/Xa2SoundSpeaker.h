﻿#pragma once
#include "../../../Include/Sound/SoundSpeaker.h"
#include <memory>

struct IXAudio2SourceVoice;
struct IXAudio2;
struct XAUDIO2_BUFFER;

namespace Eugene
{
	class SoundFile;
	class Xa2SoundSpeaker :
		public SoundSpeaker
	{
	public:
	/*	Xa2SoundSpeaker(IXAudio2* xaudio2,const Wave& wave, std::uint16_t outChannel, const float maxPitchRate);
		Xa2SoundSpeaker(IXAudio2* xaudio2, const OggVorbis& ogg, std::uint16_t outChannel, const float maxPitchRate);*/
		Xa2SoundSpeaker(IXAudio2* xaudio2, const SoundFile& soundFile, std::uint16_t outChannel, const float maxPitchRate);
		~Xa2SoundSpeaker();
	private:
		void Play(int loopCount = 0) final;
		void Stop(void) final;
		bool IsEnd(void) const final;
		void SetPitchRate(float rate) final;

		void SetVolume(float volume) final;

		void SetPan(std::span<float> volumes) final;

		void SetOutput(SoundControl& control) final;

		void SetData(const std::uint8_t* ptr, const std::uint64_t size) final;

		IXAudio2SourceVoice* source_;
		std::unique_ptr<XAUDIO2_BUFFER> buffer_;
	};
}
