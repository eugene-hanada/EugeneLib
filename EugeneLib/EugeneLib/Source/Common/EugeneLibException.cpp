#include "../../Include/Common/EugeneLibException.h"
#include "../../Include/Common/Debug.h"
#include <filesystem>

Eugene::EugeneLibException::EugeneLibException(const std::string& mess) :
	mess_{mess}
{
	std::filesystem::path tmp{ mess_ };
	DebugLog(tmp.u8string());
}

Eugene::EugeneLibException::~EugeneLibException()
{
}
const char* Eugene::EugeneLibException::what() const noexcept
{
	return mess_.c_str();
}

Eugene::LibInitException::LibInitException() :
	EugeneLibException{"初期化エラー"}
{
}

Eugene::LibReleseException::LibReleseException() :
	EugeneLibException{"解放処理エラー"}
{
}
