#pragma once
#include "../../../Include/System/DynamicLibrary.h"
#include <Windows.h>

class Dll :
	public DynamicLibrary
{
public:
	Dll(const std::filesystem::path& path);
	~Dll();
private:
	void* FindFunction(const std::string& functionName) const;

	/// <summary>
	/// DLL�̃n���h��
	/// </summary>
	HMODULE handle_;
};
