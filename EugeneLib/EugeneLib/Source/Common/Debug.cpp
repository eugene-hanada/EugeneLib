#include "../../Include/Common/Debug.h"

#ifdef _DEBUG

#include <iostream>
#include <format>
#include <filesystem>
#include <cuchar>
#include <chrono>
#ifdef _WIN64
#include <Windows.h>
FILE* fp;
#endif

Eugene::Debug& Eugene::Debug::GetInstance(void)
{
	static Debug instance_;
	return instance_;
}


void Eugene::Debug::Log(const std::string& str)
{
	binarySemphore_.acquire();
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::cout << std::chrono::zoned_time{ std::chrono::current_zone(), now } << str << std::endl;
	binarySemphore_.release();
}

Eugene::Debug::Debug() :
	binarySemphore_{1}
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