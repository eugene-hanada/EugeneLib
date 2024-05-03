#pragma once
#include "../../../Include/Sound/SoundStreamSpeaker.h"

namespace Eugene
{

	class AaudioStreamSpeaker:
        public SoundStreamSpeaker
	{
	public:
        AaudioStreamSpeaker(std::uintptr_t devicePtr, SoundStreamSpeaker& speaker, const std::filesystem::path& path);
		~AaudioStreamSpeaker();

		void Play(int loopCount = 0) final;
		void Stop(void) final;
		bool IsEnd(void) const final;
		void SetPitchRate(float rate);
		void SetOutput(SoundControl& control) final;
		void SetVolume(float volume) final;
		void SetPan(std::span<float> volumes) final;
	private:
		SoundStreamSpeaker& speaker_;
	};
}