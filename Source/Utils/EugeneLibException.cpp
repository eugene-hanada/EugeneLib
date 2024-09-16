#include "../../Include/Utils/EugeneLibException.h"

Eugene::EugeneLibException::EugeneLibException(const std::string& mess) :
	mess_{mess}
{
}

Eugene::EugeneLibException::~EugeneLibException()
{
}
const char* Eugene::EugeneLibException::what() const noexcept
{
	return mess_.c_str();
}
