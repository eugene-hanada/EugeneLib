#pragma once
#include "../../../Include/Sound/SoundStreamSpeaker.h"

namespace Eugene
{

	class SoundStreamSpeaker::SoundStreamSpeakerImpl
	{
	public:
		SoundStreamSpeakerImpl(std::uintptr_t devicePtr, SoundStreamSpeaker& speaker, const std::filesystem::path& path);
		~SoundStreamSpeakerImpl();

		void Play(int loopCount = 0);
		void Stop(void);
		bool IsEnd(void) const;
		void SetPitchRate(float rate);
		void SetOutput(SoundControl& control);
		void SetVolume(float volume);
		void SetPan(std::span<float> volumes);
	private:
		SoundStreamSpeaker& speaker_;
	};
}