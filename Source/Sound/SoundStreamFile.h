#pragma once
#include <filesystem>
#include <memory>
#include "../../Include/Sound/SoundCommon.h"

namespace Eugene
{
	/// <summary>
	/// �T�E���h�t�@�C���̃X�g���[��
	/// </summary>
	class SoundStreamFile
	{
	public:
		virtual ~SoundStreamFile();

		/// <summary>
		/// �t�H�[�}�b�g���擾
		/// </summary>
		/// <returns></returns>
		const SoundFormat& GetFormat() const;

		/// <summary>
		/// �t�H�[�}�b�g�̊g���������擾
		/// </summary>
		/// <returns></returns>
		const SoundFormatEx& GetFormatEx() const;

		/// <summary>
		/// �f�[�^�̑��T�C�Y���擾����
		/// </summary>
		/// <returns></returns>
		const std::uint64_t GetDataSize() const;

		/// <summary>
		/// �ǂݎ����s�����T�C�Y
		/// </summary>
		/// <returns></returns>
		const std::uint64_t GetReadSize() const;

		/// <summary>
		/// �ǂݎ�菈��������
		/// </summary>
		/// <param name="ptr"> �������ݐ�|�C���^ </param>
		/// <param name="size"> �ǂݍ��݃T�C�Y </param>
		virtual void Read(std::uint8_t* ptr, std::uint64_t size) = 0;

		/// <summary>
		/// �擪�ʒu�փV�[�N����
		/// </summary>
		virtual void SeekStart() = 0;
	protected:
		
		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		SoundStreamFile();

		/// <summary>
		/// �t�H�[�}�b�g
		/// </summary>
		SoundFormat format_;

		/// <summary>
		/// �t�H�[�}�b�g�̊g����
		/// </summary>
		SoundFormatEx formatEx_;

		/// <summary>
		/// �f�[�^�̑��T�C�Y
		/// </summary>
		std::uint64_t dataSize_;

		/// <summary>
		/// �ǂݎ�����T�C�Y
		/// </summary>
		std::uint64_t readSize_;
	};

	/// <summary>
	/// �w��̃t�@�C��
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	std::unique_ptr<SoundStreamFile> CreateSoundStreamFile(const std::filesystem::path& path);
}
