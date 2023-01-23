#pragma once
#include "SoundBase.h"

namespace Eugene
{
	class Wave;
	class SoundControl;

	/// <summary>
	/// サウンドを再生するクラス
	/// </summary>
	class SoundSpeaker :
		public SoundBase
	{
	public:

		virtual ~SoundSpeaker();
		virtual void Play(void) const = 0;
		virtual void Stop(void) const = 0;
		virtual bool IsEnd(void) const = 0;
		virtual void SetPitchRate(float rate) = 0;
		virtual void SetOutput(SoundControl& control) = 0;
	protected:
		SoundSpeaker(const float maxPitchRate);
		const float maxPitchRate_;
	};
}
