#include "../../Include/Utils/EugeneLibException.h"
#include "../../Include/Debug/Debug.h"
#include <filesystem>

Eugene::EugeneLibException::EugeneLibException(const std::string& mess, const std::stacktrace& stackTrace) :
	mess_{mess}, stackTrace_{stackTrace}
{
}

Eugene::EugeneLibException::~EugeneLibException()
{
}
const char* Eugene::EugeneLibException::what() const noexcept
{
	return mess_.c_str();
}
