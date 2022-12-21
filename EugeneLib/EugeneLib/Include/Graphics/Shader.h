#pragma once
#include <vector>
#include <filesystem>

namespace EugeneLib
{
	/// <summary>
	/// �V�F�[�_�[
	/// </summary>
	class Shader
	{
	public:

		/// <summary>
		/// �R���X�g���N�^(�w�肵���p�X�̃V�F�[�_�[�t�@�C����ǂݍ���)
		/// </summary>
		/// <param name="path"> �V�F�[�_�[�t�@�C���̃p�X </param>
		Shader(const std::filesystem::path& path);

		/// <summary>
		/// �ǂݍ��񂾃V�F�[�_�[�̐擪�|�C���^���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> �擪�|�C���^ </returns>
		const char* GetPtr(void) const;

		/// <summary>
		/// �V�F�[�_�[�̃T�C�Y���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> �T�C�Y </returns>
		size_t GetSize(void) const;
	private:

		/// <summary>
		/// �V�F�[�_�[�R�[�h
		/// </summary>
		std::vector<char> code_;
	};
}
