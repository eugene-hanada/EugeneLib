#include "../Include/EugeneLibException.h"


EugeneLib::EugeneLibException::EugeneLibException(const std::string& mess) :
	mess_{mess}
{
}

EugeneLib::EugeneLibException::~EugeneLibException()
{
}
const char* EugeneLib::EugeneLibException::what() const noexcept
{
	return mess_.c_str();
}

EugeneLib::LibInitException::LibInitException() :
	EugeneLibException{"èâä˙âªÉGÉâÅ["}
{
}
