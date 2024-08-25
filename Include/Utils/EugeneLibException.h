#pragma once
#include <exception>
#include <string>
#include <stacktrace>

namespace Eugene
{
	/// <summary>
	/// ���̃��C�u�����̃G�N�Z�v�V����
	/// </summary>
	class EugeneLibException :
		public std::exception
	{
	public:
		EugeneLibException(const std::string& mess, const std::stacktrace& stackTrace = {});
		~EugeneLibException();

		/// <summary>
		/// �X�^�b�N�g���[�X���擾����
		/// </summary>
		/// <returns></returns>
		const std::stacktrace& GetStackTrace() const noexcept
		{
			return stackTrace_;
		}
	private:
		const char* what() const noexcept final;

		/// <summary>
		/// ���b�Z�[�W
		/// </summary>
		std::string mess_;

		/// <summary>
		/// �X�^�b�N�g���[�X
		/// </summary>
		std::stacktrace stackTrace_;
	};
};
