#include "../../Include/Common/Debug.h"

#ifdef _DEBUG

#include <iostream>
#include <format>
#include <filesystem>
#include <cuchar>
#include <chrono>
#include <thread>

#ifdef USE_WINDOWS
#include <Windows.h>
FILE* fp;
#endif



Eugene::DebugIO& Eugene::DebugIO::GetInstance(void)
{
	static DebugIO instance_;
	return instance_;
}




void Eugene::DebugIO::Log(const std::string& str)
{
	// ロック
	binarySemphore_.acquire();

	// 現在の時間をミリ秒で取得
	auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

	// バッファを利用したストリームからスレッドIDを文字列に変換
	oss_ << std::this_thread::get_id();

	auto tmp = std::format("Log[{0:%H:%M:%S}][Thread={1:}]{2:}\n", std::chrono::zoned_time{ std::chrono::current_zone(),now }, oss_.str(), str);

	// 時間とスレッドIDとstrを出力
	std::cout << tmp;

	file_ << tmp;

	oss_.seekp(0);
	binarySemphore_.release();
}

Eugene::DebugIO::DebugIO() :
	binarySemphore_{1}, file_{ "./log.txt",std::ios::out }
{
#ifdef _WIN64

	// コンソールを開く
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
#endif

	// スレッドIDを文字列に変換してバッファのサイズにする
	oss_ << std::this_thread::get_id();
	oss_.seekp(0);
}

Eugene::DebugIO::~DebugIO()
{
#ifdef _WIN64
	fclose(fp);
#endif
}

#endif