#pragma once
#include <exception>
#include <string>

namespace Eugene
{
	/// <summary>
	/// ���̃��C�u�����̃G�N�Z�v�V����
	/// </summary>
	class EugeneLibException :
		public std::exception
	{
	public:
		EugeneLibException(const std::string& mess);
		~EugeneLibException();
	private:
		const char* what() const noexcept final;

		/// <summary>
		/// ���b�Z�[�W
		/// </summary>
		std::string mess_;
	};

	/// <summary>
	/// �������̃G���[��\���G�N�Z�v�V�����A���b�Z�[�W��[CreateError]mess�ƂȂ�
	/// </summary>
	class CreateErrorException :
		public EugeneLibException
	{
	public:
		CreateErrorException(const std::string& mess);
		~CreateErrorException();
	};
};