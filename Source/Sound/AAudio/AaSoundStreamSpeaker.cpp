#include "AaSoundStreamSpeaker.h"


Eugene::SoundStreamSpeaker::SoundStreamSpeakerImpl::SoundStreamSpeakerImpl(std::uintptr_t devicePtr, SoundStreamSpeaker& speaker, const std::filesystem::path& path) :
	 speaker_{ speaker }
{
}

Eugene::SoundStreamSpeaker::SoundStreamSpeakerImpl::~SoundStreamSpeakerImpl()
{
}

void Eugene::SoundStreamSpeaker::SoundStreamSpeakerImpl::Play(int loopCount)
{
}

void Eugene::SoundStreamSpeaker::SoundStreamSpeakerImpl::Stop(void)
{
}

bool Eugene::SoundStreamSpeaker::SoundStreamSpeakerImpl::IsEnd(void) const
{
	return true;
}

void Eugene::SoundStreamSpeaker::SoundStreamSpeakerImpl::SetPitchRate(float rate)
{
	
}

void Eugene::SoundStreamSpeaker::SoundStreamSpeakerImpl::SetOutput(SoundControl& control)
{
}

void Eugene::SoundStreamSpeaker::SoundStreamSpeakerImpl::SetVolume(float volume)
{
	
}

void Eugene::SoundStreamSpeaker::SoundStreamSpeakerImpl::SetPan(std::span<float> volumes)
{
	
}


