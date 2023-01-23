#include "../../Include/Common/Debug.h"

#ifdef _DEBUG

#include <iostream>
#include <format>
#include <filesystem>
#include <cuchar>

#ifdef _WIN64
#include <Windows.h>
FILE* fp;
#endif

Eugene::Debug& Eugene::Debug::GetInstance(void)
{
	static Debug instance_;
	return instance_;
}

void Eugene::Debug::Log(const std::u8string& log)
{
	std::filesystem::path tmp{ log };
	std::cout << tmp.string() << std::endl;
}


Eugene::Debug::Debug()
{
#ifdef _WIN64
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
#endif
}

Eugene::Debug::~Debug()
{
#ifdef _WIN64
	fclose(fp);
#endif
}

#endif