#pragma once
#include <exception>
#include <string>

namespace Eugene
{
	/// <summary>
	/// extension
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
};
