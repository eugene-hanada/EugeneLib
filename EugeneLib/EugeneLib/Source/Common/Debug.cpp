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

EugeneLib::Debug& EugeneLib::Debug::GetInstance(void)
{
	static Debug instance_;
	return instance_;
}

void EugeneLib::Debug::Log(const std::u8string& log)
{
	std::filesystem::path tmp{ log };
	std::cout << tmp.string() << std::endl;
}

void EugeneLib::Debug::Log(const std::u8string& log1, const std::u8string& log2)
{
	Log(log1 + log2);
}
//
//void EugeneLib::Debug::Log(const Vector2& vec)
//{
//	std::filesystem::path tmp{ std::format("x={0:f}y={1:f}", vec.x, vec.y) };
//	Log(tmp.u8string());
//}
//
//void EugeneLib::Debug::Log(const Vector2& vec, const std::u8string& log)
//{
//	std::filesystem::path tmp{ std::format("x={0:f}y={1:f}", vec.x, vec.y) };
//	Log(tmp.u8string(), log);
//}


EugeneLib::Debug::Debug()
{
#ifdef _WIN64
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
#endif
}

EugeneLib::Debug::~Debug()
{
#ifdef _WIN64
	fclose(fp);
#endif
}

#endif