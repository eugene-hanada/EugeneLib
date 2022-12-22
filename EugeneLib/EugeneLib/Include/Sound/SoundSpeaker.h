#pragma once

namespace EugeneLib
{
	class Wave;

	/// <summary>
	/// サウンドを再生するクラス
	/// </summary>
	class SoundSpeaker
	{
	public:
		virtual ~SoundSpeaker();
		virtual void Play(void) const = 0;
		virtual void Stop(void) const = 0;
		virtual bool IsEnd(void) const = 0;
		virtual void SetPitchRate(float rate) = 0;
	protected:
		SoundSpeaker();
	};
}
