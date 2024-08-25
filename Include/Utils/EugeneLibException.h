#pragma once
#include <exception>
#include <string>
#include <stacktrace>

namespace Eugene
{
	/// <summary>
	/// このライブラリのエクセプション
	/// </summary>
	class EugeneLibException :
		public std::exception
	{
	public:
		EugeneLibException(const std::string& mess, const std::stacktrace& stackTrace = {});
		~EugeneLibException();

		/// <summary>
		/// スタックトレースを取得する
		/// </summary>
		/// <returns></returns>
		const std::stacktrace& GetStackTrace() const noexcept
		{
			return stackTrace_;
		}
	private:
		const char* what() const noexcept final;

		/// <summary>
		/// メッセージ
		/// </summary>
		std::string mess_;

		/// <summary>
		/// スタックトレース
		/// </summary>
		std::stacktrace stackTrace_;
	};
};
