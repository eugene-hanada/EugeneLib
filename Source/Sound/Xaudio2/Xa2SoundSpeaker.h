#pragma once
#include "../../../Include/Sound/SoundSpeaker.h"
#include <xaudio2.h>
#include <memory>


namespace Eugene
{
	class SoundFile;
	class Xaudio2Speaker : 
		public SoundSpeaker
	{
	public:
		Xaudio2Speaker(IXAudio2* xaudio2, const SoundFile& soundFile, std::uint16_t outChannel, const float maxPitchRate);
		~Xaudio2Speaker();
	
		void Play(int loopCount = 0) final;
		void Stop(void) final;
		bool IsEnd(void) const final;
		void SetPitchRate(float rate) final;

		void SetVolume(float volume) final;

		void SetPan(std::span<float> volumes) final;

		void SetOutput(SoundControl& control) final;

		void SetData(const std::uint8_t* ptr, const std::uint64_t size) final;

	private:
		

		struct SourceVoiceDeleter
		{
			void operator()(IXAudio2SourceVoice* voice)
			{
				voice->DestroyVoice();
			}
		};

		std::unique_ptr<IXAudio2SourceVoice, SourceVoiceDeleter> source_;
		std::unique_ptr<XAUDIO2_BUFFER> buffer_;
	};
}
