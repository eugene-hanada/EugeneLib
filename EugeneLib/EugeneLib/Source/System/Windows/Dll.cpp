#include "Dll.h"

Dll::Dll(const std::filesystem::path& path)
{
	handle_ = LoadLibrary(TEXT("Dll.dll"));
}

Dll::~Dll()
{
	FreeLibrary(handle_);
}

void* Dll::FindFunction(const std::string& functionName) const
{
	return GetProcAddress(handle_,functionName.c_str());
}
