#pragma once
#include <xaudio2.h>
#include <memory>
#include "../SoundBase.h"
#include "../../../Include/Debug/Debug.h"


namespace Eugene
{
	class SoundFile;

	class SoundSpeaker :
		public SoundBase
	{
	public:
		SoundSpeaker() :
			maxPitchRate_{2.0f}
		{

		}
		~SoundSpeaker();
	
		void Play(int loopCount = 0);
		void Stop(void) noexcept;
		bool IsEnd(void) const;
		void SetPitchRate(float rate);

		void SetVolume(float volume) final;

		void SetPan(std::span<float> volumes) final;

		void SetOutput(SoundControl& control) final;

		void SetData(const std::uint8_t* ptr, const std::uint64_t size);

		SoundSpeaker(SoundSpeaker&& speaker) noexcept :
			SoundBase{ std::move(speaker) }, source_{ std::move(speaker.source_) }, buffer_{ std::move(speaker.buffer_) }, maxPitchRate_{speaker.maxPitchRate_}
		{
		}

		SoundSpeaker& operator=(SoundSpeaker&& speaker) noexcept
		{
			static_cast<SoundBase&>(*this) = std::move(speaker);
			source_ = std::move(speaker.source_);
			buffer_ = std::move(speaker.buffer_);
			EUGENE_ASSERT_MSG(static_cast<bool>(speaker.maxPitchRate_ <= maxPitchRate_), "ピッチレートの最大値は大きくすることはできません");
			return *this;
		}

		void Final() noexcept
		{
			buffer_.reset();
			source_->Stop();
			source_.reset();
		}

		SoundSpeaker(const SoundSpeaker&) = delete;
		SoundSpeaker& operator=(const SoundSpeaker&) = delete;
	private:
		SoundSpeaker(IXAudio2* xaudio2, const SoundFile& soundFile, std::uint16_t outChannel, const float maxPitchRate);

		struct SourceVoiceDeleter
		{
			void operator()(IXAudio2SourceVoice* voice)
			{
				voice->DestroyVoice();
			}
		};

		std::unique_ptr<IXAudio2SourceVoice, SourceVoiceDeleter> source_;
		std::unique_ptr<XAUDIO2_BUFFER> buffer_;
		const float maxPitchRate_;
		friend class Sound;
	};
}
