#include "Dll.h"
#include "../../../Include/Common/EugeneLibException.h"

Eugene::Dll::Dll(const std::filesystem::path& path)
{
	// DLLを読み込む
#ifdef UNICODE
	handle_ = LoadLibrary(path.wstring().c_str());
#else
	handle_ = LoadLibrary(path.string().c_str());
#endif

	if (handle_ == nullptr)
	{
		throw CreateErrorException{ "Dllロード失敗" + path.string() };
	}

}

Eugene::Dll::~Dll()
{
	// DLLを解放する
	FreeLibrary(handle_);
}

void* Eugene::Dll::FindFunction(const std::string& functionName) const
{
	// 指定の名前の関数を取得し返す
	return GetProcAddress(handle_,functionName.c_str());
}
