#include "../../Include/Common/EugeneLibException.h"
#include <format>
#include "../../Include/Common/Debug.h"
#include <filesystem>

Eugene::EugeneLibException::EugeneLibException(const std::string& mess) :
	mess_{mess}
{
	// ログにメッセージを出す
	DebugLog(mess_);
}

Eugene::EugeneLibException::~EugeneLibException()
{
}
const char* Eugene::EugeneLibException::what() const noexcept
{
	return mess_.c_str();
}

Eugene::CreateErrorException::CreateErrorException(const std::string& mess) :
	EugeneLibException{std::format("[CreateError]{}",mess)}
{
}

Eugene::CreateErrorException::~CreateErrorException()
{
}
