#pragma once
#include "../../../Include/Sound/SoundSpeaker.h"
#include <xaudio2.h>
#include <memory>


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
		

		struct SourceVoiceDeleter
		{
			void operator()(IXAudio2SourceVoice* voice)
			{
				voice->DestroyVoice();
			}
		};

		/// <summary>
		/// 
		/// </summary>
		SoundSpeaker& speaker_;

		std::unique_ptr<IXAudio2SourceVoice, SourceVoiceDeleter> source_;
		std::unique_ptr<XAUDIO2_BUFFER> buffer_;
	};
}
