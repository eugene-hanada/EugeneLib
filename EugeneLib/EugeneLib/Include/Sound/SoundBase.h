#pragma once
#include <cstdint>
#include <span>

namespace EugeneLib
{
	class SoundBase
	{
	public:
		virtual ~SoundBase();

		/// <summary>
		/// �S�̂̃{�����[�����Z�b�g����
		/// </summary>
		/// <param name="volume"> �{�����[�� </param>
		virtual void SetVolume(float volume) = 0;

		/// <summary>
		/// �p���j���O�p�Ƀ{�����[�����Z�b�g����
		/// </summary>
		/// <param name="volumes"></param>
		virtual void SetPan(std::span<float> volumes) = 0;

		/// <summary>
		/// ���݂̃{�����[�����擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		float GetVolume(void) const;

		/// <summary>
		/// �o�̓`�����l�����擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		std::uint16_t GetOututChannel(void) const;

	protected:
		SoundBase();

		/// <summary>
		/// ���݂̃{�����[��
		/// </summary>
		float volume_;

		/// <summary>
		/// �o�̓`�����l��
		/// </summary>
		std::uint16_t outChannel_;

		/// <summary>
		/// ���̓`�����l��
		/// </summary>
		std::uint16_t inChannel_;
	};

}