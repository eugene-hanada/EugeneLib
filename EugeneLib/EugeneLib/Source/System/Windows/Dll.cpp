#include "Dll.h"

Dll::Dll(const std::filesystem::path& path)
{
	// DLL‚ğ“Ç‚İ‚Ş
	handle_ = LoadLibrary(TEXT("Dll.dll"));
}

Dll::~Dll()
{
	// DLL‚ğ‰ğ•ú‚·‚é
	FreeLibrary(handle_);
}

void* Dll::FindFunction(const std::string& functionName) const
{
	// w’è‚Ì–¼‘O‚ÌŠÖ”‚ğæ“¾‚µ•Ô‚·
	return GetProcAddress(handle_,functionName.c_str());
}
