#pragma once
#include <cstdint>
#include <filesystem>
#include "SoundCommon.h"

namespace Eugene
{
	class SoundFile
	{
	public:
		virtual ~SoundFile();
		/// <summary>
		/// �t�H�[�}�b�g������ǂݍ��݂܂�
		/// </summary>
		/// <param name=""></param>
		virtual void LoadFormat(void) = 0;

		/// <summary>
		/// �f�[�^������ǂݍ��݂܂�
		/// </summary>
		/// <param name=""></param>
		virtual void LoadData(void) = 0;

		/// <summary>
		/// �t�@�C�������
		/// </summary>
		/// <param name=""></param>
		virtual void Close(void) = 0;

		/// <summary>
		/// �f�[�^�ւ̃|�C���^���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint8_t* GetDataPtr(void) const;

		/// <summary>
		/// �f�[�^�̃o�C�g�����擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint64_t GetDataSize(void) const;

		const SoundFormat& GetFormat(void) const;

		const SoundFormatEx& GetFormatExt(void) const;
	protected:
		SoundFile();
		std::vector<std::uint8_t> data_;
		SoundFormat format_;
		SoundFormatEx ex_;
	};

	/// <summary>
	/// �T�E���h�t�@�C�����J���C���^�[�t�F�[�X�N���X�𐶐�����
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	SoundFile* OpenSoundFile(const std::filesystem::path& path);
}