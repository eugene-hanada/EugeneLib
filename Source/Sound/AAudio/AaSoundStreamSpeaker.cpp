#include "AaSoundStreamSpeaker.h"


Eugene::AaudioStreamSpeaker::AaudioStreamSpeaker(std::uintptr_t devicePtr, SoundStreamSpeaker& speaker, const std::filesystem::path& path) :
        SoundStreamSpeaker{0,0.0f},speaker_{ speaker }
{
}

Eugene::AaudioStreamSpeaker::~AaudioStreamSpeaker()
{
}

void Eugene::AaudioStreamSpeaker::Play(int loopCount)
{
}

void Eugene::AaudioStreamSpeaker::Stop(void)
{
}

bool Eugene::AaudioStreamSpeaker::IsEnd(void) const
{
	return true;
}

void Eugene::AaudioStreamSpeaker::SetPitchRate(float rate)
{
	
}

void Eugene::AaudioStreamSpeaker::SetOutput(SoundControl& control)
{
}

void Eugene::AaudioStreamSpeaker::SetVolume(float volume)
{
	
}

void Eugene::AaudioStreamSpeaker::SetPan(std::span<float> volumes)
{
	
}


