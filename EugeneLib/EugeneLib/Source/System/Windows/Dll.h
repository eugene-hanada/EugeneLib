#pragma once
#include "DynamicLibrary.h"
#include <Windows.h>

class Dll :
	public DynamicLibrary
{
public:
	Dll(const std::filesystem::path& path);
	~Dll();
private:
	void* FindFunction(const std::string& functionName) const;

	HMODULE handle_;
};

