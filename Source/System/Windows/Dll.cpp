#include "Dll.h"

Dll::Dll(const std::filesystem::path& path)
{
	// DLLを読み込む
	handle_ = LoadLibrary(TEXT("Dll.dll"));
}

Dll::~Dll()
{
	// DLLを解放する
	FreeLibrary(handle_);
}

void* Dll::FindFunction(const std::string& functionName) const
{
	// 指定の名前の関数を取得し返す
	return GetProcAddress(handle_,functionName.c_str());
}
