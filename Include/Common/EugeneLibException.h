#pragma once
#include <exception>
#include <string>

namespace Eugene
{
	/// <summary>
	/// 子のライブラリのエクセプション
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
	/// 生成時のエラーを表すエクセプション
	/// </summary>
	class CreateErrorException :
		public EugeneLibException
	{
	public:
		CreateErrorException(const std::string& mess);
		~CreateErrorException();
	};
};
