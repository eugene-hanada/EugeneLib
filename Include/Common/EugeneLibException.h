#pragma once
#include <exception>
#include <string>

namespace Eugene
{
	/// <summary>
	/// このライブラリのエクセプション
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
		/// メッセージ
		/// </summary>
		std::string mess_;
	};

	/// <summary>
	/// 生成時のエラーを表すエクセプション、メッセージが[CreateError]messとなる
	/// </summary>
	class CreateErrorException :
		public EugeneLibException
	{
	public:
		CreateErrorException(const std::string& mess);
		~CreateErrorException();
	};
};
