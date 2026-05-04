#include "../../Include/Debug/Debug.h"

#include <cuchar>
#include <chrono>
#include <thread>
#include <ostream>
#include <fstream>
#include <iostream>

#ifdef EUGENE_TEST
namespace
{
	std::ostream& os{ std::cout };
}
#else
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
#elifdef EUGENE_ANDROID
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

#ifdef EUGENE_ANDROID
    os_{androidOs}
#elif EUGENE_WINDOWS
	os_{ windowsOs }
#endif
{
	// スレッドIDを文字列に変換してバッファのサイズにする
	oss_ << std::this_thread::get_id();
	oss_.seekp(0);
	filter_.flip();
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


void Eugene::Debug::Out(Type type, const std::string_view& string)
{
	binarySemphore_.acquire();
	// 秒の小数点4桁（00.xxxx）を出力するため、フォーマットに小数部プレースホルダを追加
	constexpr auto format = "[{0:%Y/%m/%d/%H:%M-%S}.{3:04}][Thread={1:}]{2:}";
	std::bitset<4> tmp;
	tmp.set(static_cast<std::size_t>(type), true);
	if ((filter_ & tmp).any())
	{
		// 現在時刻を取得（秒単位切り捨て）とその小数部を計算
		auto now = std::chrono::system_clock::now();
		auto sec = std::chrono::floor<std::chrono::seconds>(now);
		auto z = std::chrono::zoned_seconds{ std::chrono::current_zone(), sec };

		// 小数部を 4 桁 (1/10000秒単位) にする：
		// nanoseconds / 100000 -> 0..9999
		auto sub_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - sec).count();
		int frac4 = static_cast<int>((sub_ns / 100000) % 10000);

		// バッファを利用したストリームからスレッドIDを文字列に変換
		oss_ << std::this_thread::get_id();
		auto formatedString = std::format(format, z, oss_.str(), string, frac4);
		os_ << formatedString << std::endl;

		if (logStringBuffer_.size() > 0)
		{
			logStringBuffer_ += '\n';
		}

		logStringBuffer_ += formatedString;
		oss_.seekp(0);
	}
	binarySemphore_.release();
}
