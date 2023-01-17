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

	class LibReleseException :
		EugeneLibException
	{
	public:
		LibReleseException();
	};
};

#ifndef NOT_USE_EXCEPTION
#define ThrowFalse(cd, mess) ( !cd ? throw EugeneLib::EugeneLibException{mess}: )
#else
#define ThrowFalse(cd, mess) ()
#endif
