#pragma once
#include <filesystem>
#include <vector>
#include "SoundCommon.h"


namespace EugeneLib
{
	// Wave�N���X
	class Wave
	{
	public:
		Wave(const std::filesystem::path& path);

		/// <summary>
		/// fmt�̕������擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const SoundFormat& GetFmt(void) const&
		{
			return fmt_;
		}

		/// <summary>
		/// �f�[�^���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::vector<unsigned char>& GetData(void) const&
		{
			return data_;
		}

	private:

		/// <summary>
		/// Fmt�`�����N�̓ǂݍ���
		/// </summary>
		/// <param name="file"></param>
		void LoadFmt(std::ifstream& file);

		/// <summary>
		/// �f�[�^�`�����N�̓ǂݍ���
		/// </summary>
		/// <param name="file"></param>
		void LoadData(std::ifstream& file);


		// �f�[�^
		std::vector<unsigned char> data_;

		// fmt
		SoundFormat fmt_;

		// RiFF�`�����N
		struct RIFF
		{
			char id[4];
			unsigned int size;
			char type[4];
		};

	};
}
