#pragma once
#include "../../../Include/Sound/SoundSpeaker.h"
#include <memory>

struct IXAudio2SourceVoice;
struct IXAudio2;
struct XAUDIO2_BUFFER;

namespace Eugene
{
	class SoundFile;
	class SoundSpeaker::SoundSpeakerImpl
	{
	public:
		SoundSpeakerImpl(std::uintptr_t device, SoundSpeaker& speaker, const SoundFile& soundFile);
		~SoundSpeakerImpl();
	
		void Play(int loopCount = 0) ;
		void Stop(void) ;
		bool IsEnd(void) const ;
		void SetPitchRate(float rate) ;

		void SetVolume(float volume) ;

		void SetPan(std::span<float> volumes) ;

		void SetOutput(SoundControl& control) ;

		void SetData(const std::uint8_t* ptr, const std::uint64_t size) ;

	private:
		/// <summary>
		/// 
		/// </summary>
		SoundSpeaker& speaker_;

		IXAudio2SourceVoice* source_;
		std::unique_ptr<XAUDIO2_BUFFER> buffer_;
	};
}
