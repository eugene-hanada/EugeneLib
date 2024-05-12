#include "../../Include/Utils/EugeneLibException.h"
#include "../../Include/Debug/Debug.h"
#include <filesystem>

Eugene::EugeneLibException::EugeneLibException(const std::string& mess) :
	mess_{mess}
{
	DebugIO.Error("Exception {}", mess_);
}

Eugene::EugeneLibException::~EugeneLibException()
{
}
const char* Eugene::EugeneLibException::what() const noexcept
{
	return mess_.c_str();
}

Eugene::CreateErrorException::CreateErrorException(const std::string& mess) :
	EugeneLibException{fmt::format("[CreateError]{}",mess)}
{
}

Eugene::CreateErrorException::~CreateErrorException()
{
}
