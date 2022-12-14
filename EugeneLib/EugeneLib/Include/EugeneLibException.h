#pragma once
#include <exception>
#include <string>

namespace EugeneLib
{

	class EugeneLibException :
		public std::exception
	{
	public:
		EugeneLibException(const std::string& mess);
		~EugeneLibException();
	private:
		const char* what() const noexcept final;
		std::string mess_;
	};

	class LibInitException :
		EugeneLibException
	{
	public:
		LibInitException();
	};
};
