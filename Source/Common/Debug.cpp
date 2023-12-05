#include "../../Include/Common/Debug.h"
#ifdef _DEBUG

#include <cuchar>
#include <chrono>
#include <thread>
#include <ostream>
#include <fstream>

#ifdef USE_WINDOWS
#include <Windows.h>
#endif

namespace
{
	FILE* fp = nullptr;
	constexpr std::string_view names[]{ "ERROR","WARNING","LOG","DEBUG" };
}


Eugene::Debug& Eugene::Debug::GetInstance(void)
{
	static Debug instance_;
	return instance_;
}

Eugene::Debug::Debug() :
	binarySemphore_{1}, spanStream_{buffer_}, filter_{0u}, exportPath_{"./Log.txt"}
{
	// スレッドIDを文字列に変換してバッファのサイズにする
	oss_ << std::this_thread::get_id();
	oss_.seekp(0);

	buffer_.resize(256ull);
	spanStream_ = std::spanstream{ buffer_ };

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
	filter_.set(std::to_underlying(type), true);
}

void Eugene::Debug::RemoveFilter(Type type)
{
	filter_.set(std::to_underlying(type), false);
}

std::string_view Eugene::Debug::GetBuffer_View(void)
{
	return std::string_view{buffer_.data(), static_cast<size_t>(spanStream_.tellp()) };
}

void Eugene::Debug::ClearBuffer(void)
{
	std::fill(buffer_.begin(), buffer_.end(), '\0');
	std::streampos pos{0};
	spanStream_.seekp(pos);
}

void Eugene::Debug::OpenConsole(void)
{
#ifdef _WIN64

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

	size_t pos = spanStream_.tellp();
	file << std::string_view{buffer_.data(),pos };
}

#endif

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
	// 現在の位置
	size_t pos = spanStream_.tellp();

	// フォーマット後ののサイズ
	constexpr auto fmtSize = std::size("WARNING[HH:MM:SSSSSS][Thread=00000]");

	if ((string.length() + fmtSize + pos) >= buffer_.size())
	{
		// 書き込んだ際バッファよりサイズが超える場合リサイズする
		buffer_.resize(buffer_.size() * 2ull);
		spanStream_ = std::spanstream{ buffer_ };
		spanStream_.seekp(pos);
	}
}

void Eugene::Debug::Out(Type type, const std::string_view& string)
{
	binarySemphore_.acquire();
	constexpr auto fmt = "{0:}[{1:%H:%M:%S}][Thread={2:}]{3:}";
	std::bitset<4> tmp;
	tmp.set(std::to_underlying(type), true);
	if ((filter_ & tmp).any())
	{
		CheckBuffer(string);
		
		// 現在時刻を
		auto now = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()) };

		// バッファを利用したストリームからスレッドIDを文字列に変換
		oss_ << std::this_thread::get_id();
		std::println(fmt, type, now, oss_.view(), string);
		std::println(spanStream_, fmt, names[std::to_underlying(type)], now, oss_.view(), string);
		oss_.seekp(0);
	}
	binarySemphore_.release();
}