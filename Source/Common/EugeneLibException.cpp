#include "../../Include/Common/EugeneLibException.h"
#include "../../Include/Common/Debug.h"
#include <filesystem>

Eugene::EugeneLibException::EugeneLibException(const std::string& mess) :
	mess_{mess}
{
	DebugLog(mess_);
}

Eugene::EugeneLibException::~EugeneLibException()
{
}
const char* Eugene::EugeneLibException::what() const noexcept
{
	return mess_.c_str();
}

Eugene::LibInitException::LibInitException() :
	EugeneLibException{"�������G���["}
{
}

Eugene::LibReleseException::LibReleseException() :
	EugeneLibException{"��������G���["}
{
}