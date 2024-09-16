#include "../../../Include/System/Windows/Dll.h"
#include "../../../Include/Utils/EugeneLibException.h"

Eugene::DynamicLibrary::DynamicLibrary(const std::filesystem::path& path)
{
	// DLLを読み込む
#ifdef UNICODE
	handle_ = LoadLibrary(path.wstring().c_str());
#else
	handle_ = LoadLibrary(path.string().c_str());
#endif

	if (handle_ == nullptr)
	{
		throw EugeneLibException{ "Dllロード失敗" + path.string() };
	}
}

Eugene::DynamicLibrary::~DynamicLibrary()
{
	// DLLを解放する
	FreeLibrary(handle_);
}

void* Eugene::DynamicLibrary::FindFunction(const std::string_view& functionName) const
{
	// 指定の名前の関数を取得し返す
	return GetProcAddress(handle_,functionName.data());
}
