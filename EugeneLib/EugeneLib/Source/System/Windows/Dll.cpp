#include "Dll.h"

Dll::Dll(const std::filesystem::path& path)
{
	// DLL��ǂݍ���
	handle_ = LoadLibrary(TEXT("Dll.dll"));
}

Dll::~Dll()
{
	// DLL���������
	FreeLibrary(handle_);
}

void* Dll::FindFunction(const std::string& functionName) const
{
	// �w��̖��O�̊֐����擾���Ԃ�
	return GetProcAddress(handle_,functionName.c_str());
}
