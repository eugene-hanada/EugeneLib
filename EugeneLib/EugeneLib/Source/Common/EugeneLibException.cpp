#include "../../Include/Common/EugeneLibException.h"
#include "../../Include/Common/Debug.h"
#include <filesystem>

EugeneLib::EugeneLibException::EugeneLibException(const std::string& mess) :
	mess_{mess}
{
	std::filesystem::path tmp{ mess_ };
	DebugLog(tmp.u8string());
}

EugeneLib::EugeneLibException::~EugeneLibException()
{
}
const char* EugeneLib::EugeneLibException::what() const noexcept
{
	return mess_.c_str();
}

EugeneLib::LibInitException::LibInitException() :
	EugeneLibException{"初期化エラー"}
{
}

EugeneLib::LibReleseException::LibReleseException() :
	EugeneLibException{"解放処理エラー"}
{
}
