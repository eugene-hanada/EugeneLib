#include "../../Include/Debug/Debug.h"


#include "fmt/chrono.h"
#include <cuchar>
#include <chrono>
#include <thread>
#include <ostream>
#include <fstream>
#include <iostream>


#ifdef EUGENE_WINDOWS
#include <Windows.h>

namespace
{
	class WindowsOut :
		public std::stringbuf
	{
	public:
		int sync() final {
			std::printf("%s",str().c_str());
			OutputDebugStringA(str().c_str());
			str("");
			return 0;
		}
	};
	auto windowsOut{ WindowsOut{} };
	std::ostream windowsOs{ &windowsOut };
}

#endif

#ifdef EUGENE_ANDROID
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
	binarySemphore_{1},  filter_{0u}, exportPath_{"./Log.txt"}, isExport_{true},

#ifdef USE_ANDROID
    os_{androidOs}
#else
	os_{ windowsOs }
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

void Eugene::Debug::ClearBuffer(void)
{
	logStringBuffer_.clear();
}

void Eugene::Debug::OpenConsole(void)
{
#ifdef EUGENE_WINDOWS

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

	if (isExport_)
	{
		if (!exportPath_.has_filename())
		{
			exportPath_ += "Log.txt";
		}
		std::ofstream file{ exportPath_ };
		if (!file)
		{
			return;
		}

		file << logStringBuffer_;
	}
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


void Eugene::Debug::Out(Type type, const std::string_view& string)
{
	binarySemphore_.acquire();
	constexpr auto format = "{0:}[{1:%H:%M:%S}][Thread={2:}]{3:}\0";
	std::bitset<4> tmp;
	tmp.set(static_cast<std::size_t>(type), true);
	if ((filter_ & tmp).any())
	{
		// 現在時刻を
		std::time_t t = std::time(nullptr);

		// バッファを利用したストリームからスレッドIDを文字列に変換
		oss_ << std::this_thread::get_id();
		os_ << fmt::format(format, names[static_cast<std::size_t>(type)], fmt::localtime(t), oss_.str(), string) << std::endl;
		
		if (logStringBuffer_.size() > 0)
		{
			logStringBuffer_.erase(logStringBuffer_.size() - 1);
			logStringBuffer_ += '\n';
		}
		
		auto finalString = fmt::format(format, names[static_cast<std::size_t>(type)], fmt::localtime(t), oss_.str(), string);
		logStringBuffer_ += finalString;
		oss_.seekp(0);
	}
	binarySemphore_.release();
}