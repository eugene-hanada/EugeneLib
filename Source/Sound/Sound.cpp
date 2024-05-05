#include "../../Include/Sound/Sound.h"

#ifdef USE_WINDOWS
#include "Xaudio2/Xa2Sound.h"
#elif USE_ANDROID
#include "AAudio/AaSound.h"
#endif
#include "../../Include/Utils/EugeneLibException.h"
namespace {
	bool isCreate = false;
}

Eugene::Sound::~Sound()
{
	isCreate = false;
}

Eugene::Sound::Sound() :
	sampleRate_{0u}
{
	//impl_ = std::make_unique<SoundImpl>(*this);
}

//Eugene::SoundSpeaker* Eugene::Sound::CreateSoundSpeaker(const SoundFile& soundFile, const float maxPitchRate) const
//{
//	return impl_->CreateSoundSpeaker(soundFile,maxPitchRate);
//}
//
//Eugene::SoundStreamSpeaker* Eugene::Sound::CreateSoundStreamSpeaker(const std::filesystem::path& path, const float maxPitchRate) const
//{
//	return impl_->CreateSoundStreamSpeaker(path,maxPitchRate);
//}
//
//Eugene::SoundControl* Eugene::Sound::CreateSoundControl(std::uint32_t stage, std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel) const
//{
//	return impl_->CreateSoundControl(stage,sample,inputChannel,outChannel);
//}
//
//Eugene::Sound3DControl* Eugene::Sound::CreateSound3DControl(std::uint32_t stage, std::uint32_t sample, std::uint16_t inputChannel, std::uint16_t outChannel) const
//{
//	return impl_->CreateSound3DControl(stage,sample,inputChannel,outChannel);
//}
//
//void Eugene::Sound::SetVolume(float volume)
//{
//	impl_->SetVolume(volume);
//}
//
//void Eugene::Sound::SetPan(std::span<float> volumes)
//{
//	impl_->SetPan(volumes);
//}

Eugene::Sound* Eugene::CreateSound(void)
{
	if (isCreate)
	{
		throw CreateErrorException{"すでにSoundは生成されています"};
	}
	isCreate = true;
#ifdef USE_WINDOWS
	return new Xaudio2Sound{};
#elif USE_ANDROID
    return new AaudioSound{};
#endif
}

Eugene::UniqueSound Eugene::CreateSoundUnique(void)
{
	return UniqueSound{ CreateSound()};
}
