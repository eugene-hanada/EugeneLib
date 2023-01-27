#pragma once
#include <filesystem>
#include <vector>
#include "GraphicsCommon.h"

namespace Eugene
{
	/// <summary>
	/// �e�N�X�`��
	/// </summary>
	class Texture
	{
	public:
		
		/// <summary>
		/// �R���X�g���N�^(�w�肵���e�N�X�`���t�@�C����ǂݍ���)
		/// </summary>
		/// <param name="path"> �e�N�X�`���t�@�C���̃p�X </param>
		Texture(const std::filesystem::path& path);

		/// <summary>
		/// �e�N�X�`���̏����擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const TextureInfo& GetInfo(void) const&;


		const std::vector<std::uint8_t>& GetData(void) const&;

		/// <summary>
		/// �e�N�X�`���f�[�^���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> �e�N�X�`���f�[�^ </returns>
		std::uint8_t* GetData(void)&;
	private:

		/// <summary>
		/// �e�N�X�`���f�[�^
		/// </summary>
		std::vector<std::vector<std::uint8_t>> data_;

		/// <summary>
		/// �e�N�X�`�����
		/// </summary>
		TextureInfo info_;
	};
}
