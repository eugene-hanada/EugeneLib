#pragma once
#include <cstdint>
#include <filesystem>
#include <vector>
#include <bit>
#include "SoundCommon.h"

namespace Eugene
{
	/// <summary>
	/// �T�E���h�t�@�C����ǂݍ��ނ��߂̃N���X
	/// </summary>
	class SoundFile
	{
	public:
		static constexpr int fmt = std::endian::native == std::endian::big ? 'fmt ' : ' tmf';
		static constexpr int data = std::endian::native == std::endian::big ? 'data' : 'atad';

		struct RIFF
		{
			char id[4];
			unsigned int size;
			char type[4];
		};

		SoundFile(const std::filesystem::path& path);
		SoundFile(SoundFile&& soundFile);

		SoundFile& operator=(SoundFile&& soundFile);

		/// <summary>
		/// �f�[�^�ւ̃|�C���^���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint8_t* GetDataPtr(void) const noexcept;

		/// <summary>
		/// �f�[�^�̃o�C�g�����擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint64_t GetDataSize(void) const noexcept;

		/// <summary>
		/// �t�H�[�}�b�g�̎擾
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const SoundFormat& GetFormat(void) const noexcept;

		/// <summary>
		/// �t�H�[�}�b�g�̊g���f�[�^���擾
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const SoundFormatEx& GetFormatExt(void) const noexcept;
	protected:

		bool LoadWave(const std::filesystem::path& path);

		bool LoadOggVorbis(const std::filesystem::path& path);

		/// <summary>
		/// �f�[�^��
		/// </summary>
		std::vector<std::uint8_t> data_;

		/// <summary>
		/// �t�H�[�}�b�g
		/// </summary>
		SoundFormat format_;

		/// <summary>
		/// �g���f�[�^����
		/// </summary>
		SoundFormatEx ex_;
	};
}