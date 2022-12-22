#pragma once
#include "../../../Include/Sound/SoundSpeaker.h"
#include <memory>

struct IXAudio2SourceVoice;
struct IXAudio2;
struct XAUDIO2_BUFFER;

namespace EugeneLib
{

	class Xa2SoundSpeaker :
		public SoundSpeaker
	{
	public:
		Xa2SoundSpeaker(IXAudio2* xaudio2,const Wave& wave);
		~Xa2SoundSpeaker();
	private:
		void Play(void) const final;
		void Stop(void) const final;
		bool IsEnd(void) const final;
		void SetPitchRate(float rate) final;
		IXAudio2SourceVoice* source_;
		std::unique_ptr<XAUDIO2_BUFFER> buffer_;
	};
}
