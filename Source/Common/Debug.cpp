#include "../../Include/Common/Debug.h"


#include "fmt/chrono.h"
#include <cuchar>
#include <chrono>
#include <thread>
#include <ostream>
#include <fstream>
#include <iostream>


#ifdef USE_WINDOWS
#include <Windows.h>
#endif

#ifdef USE_ANDROID
#include <android/log.h>

namespace
{
    class AndroidOut :
            public std::stringbuf
    {
    public:
        int sync() final {
            __android_log_print(ANDROID_LOG_INFO, "EugeneLibLog", "%s", str().c_str());
            str("");
            return 0;
        }
    };
    auto androidOut{AndroidOut{}};
    std::ostream androidOs{&androidOut};
}
#endif

namespace
{
	FILE* fp = nullptr;
	constexpr std::string_view names[]{ "ERROR","WARNING","LOG","DEBUG" };
	constexpr std::string_view colorNames[]{ "\x1B[31;1mERROR\x1B[37;m","\x1B[33;1mWARNING\x1B[37;m","LOG","\x1B[34;1mDEBUG\x1B[37;m" };
}





Eugene::Debug& Eugene::Debug::GetInstance(void)
{
	static Debug instance_;
	return instance_;
}

Eugene::Debug::Debug() :
	binarySemphore_{1},  filter_{0u}, exportPath_{"./Log.txt"},

#ifdef USE_ANDROID
    os_{androidOs}
#else
	os_{std::cout}
#endif
{
	// スレッドIDを文字列に変換してバッファのサイズにする
	oss_ << std::this_thread::get_id();
	oss_.seekp(0);

	filter_.flip();
}

void Eugene::Debug::ClearFillter(bool flag)
{
	filter_ = 0ull;
	if (flag)
	{
		filter_.flip();
	}
}

void Eugene::Debug::AddFilter(Type type)
{
	filter_.set(static_cast<std::size_t>(type), true);
}

void Eugene::Debug::RemoveFilter(Type type)
{
	filter_.set(static_cast<std::size_t>(type), false);
}

std::string_view Eugene::Debug::GetBuffer_View(void)
{
#ifndef USE_ANDROID
    return ss_.view();
#else
	return ss_.str();
#endif
}

void Eugene::Debug::ClearBuffer(void)
{
	ss_.fill('\0');
	std::streampos pos{0};
	ss_.seekp(pos);
}

void Eugene::Debug::OpenConsole(void)
{
#ifdef USE_WINDOWS

	// コンソールを開く
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	auto h = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consoleMode = 0;
	GetConsoleMode(h, &consoleMode);
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(h, consoleMode);
#endif
}

void Eugene::Debug::SetExportPath(const std::filesystem::path& path)
{
	exportPath_ =  path;
}


Eugene::Debug::~Debug()
{
#ifndef USE_ANDROID
	if (fp != nullptr)
	{
		fclose(fp);
	}

	if (!exportPath_.has_filename())
	{
		exportPath_ += "Log.txt";
	}
	std::ofstream file{ exportPath_ };
	if (!file)
	{
		return;
	}

	file << ss_.view();
#endif
}


void Eugene::Debug::Log(const std::string_view& string)
{
	Out(Type::Log, string);
}

void Eugene::Debug::Error(const std::string_view& string)
{
	Out(Type::Error, string);
}

void Eugene::Debug::Warning(const std::string_view& string)
{
	Out(Type::Warning, string);
}

void Eugene::Debug::LogDebug(const std::string_view& string)
{
	Out(Type::Debug, string);
}

void Eugene::Debug::CheckBuffer(const std::string_view& string)&
{
	//// 現在の位置
	//size_t pos = spanStream_.tellp();

	//// フォーマット後ののサイズ
	//constexpr auto fmtSize = std::size("WARNING[HH:MM:SSSSSS][Thread=00000]");

	//if ((string.length() + fmtSize + pos) >= buffer_.size())
	//{
	//	// 書き込んだ際バッファよりサイズが超える場合リサイズする
	//	buffer_.resize(buffer_.size() * 2ull);
	//	spanStream_ = std::spanstream{ buffer_ };
	//	spanStream_.seekp(pos);
	//}
}

void Eugene::Debug::Out(Type type, const std::string_view& string)
{
	binarySemphore_.acquire();
	constexpr auto format = "{0:}[{1:%H:%M:%S}][Thread={2:}]{3:}";
	std::bitset<4> tmp;
	tmp.set(static_cast<std::size_t>(type), true);
	if ((filter_ & tmp).any())
	{
		//CheckBuffer(string);

		// 現在時刻を
		//auto now = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()) };
		std::time_t t = std::time(nullptr);

		// バッファを利用したストリームからスレッドIDを文字列に変換
		oss_ << std::this_thread::get_id();
		os_ << fmt::format("{0:}[{1:%H:%M:%S}][Thread={2:}]{3:}", colorNames[static_cast<std::size_t>(type)], fmt::localtime(t), oss_.str(), string) << std::endl;
		ss_ << fmt::format("{0:}[{1:%H:%M:%S}][Thread={2:}]{3:}", names[static_cast<std::size_t>(type)], fmt::localtime(t), oss_.str(), string);
		oss_.seekp(0);
	}
	binarySemphore_.release();
}