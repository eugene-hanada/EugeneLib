#pragma once

namespace EugeneLib
{
	class SoundSpeaker;
	class Wave;
	/// <summary>
	/// �T�E���h�p�̃N���X
	/// </summary>
	class Sound
	{
	public:
		virtual ~Sound();

		/// <summary>
		/// �T�E���h�Đ�����N���X���쐬����
		/// </summary>
		/// <param name="wave"></param>
		/// <returns></returns>
		virtual SoundSpeaker* CreateSoundSpeaker(const Wave& wave) const = 0;

		/// <summary>
		/// �S�̂̃{�����[�����Z�b�g����
		/// </summary>
		/// <param name="volume"> �{�����[�� </param>
		virtual void SetVolume(float volume) = 0;

	protected:
		Sound();
	};

	Sound* CreateSound(void);
}
