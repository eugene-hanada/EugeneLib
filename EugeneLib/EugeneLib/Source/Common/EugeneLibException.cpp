#include "../../Include/Common/EugeneLibException.h"


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
	EugeneLibException{"初期化エラー"}
{
}

EugeneLib::LibReleseException::LibReleseException() :
	EugeneLibException{"解放処理エラー"}
{
}
