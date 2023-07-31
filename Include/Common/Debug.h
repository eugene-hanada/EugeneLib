#pragma once
#include <concepts>
#include <string>
#include <filesystem>
#include <format>
#include <semaphore>


#if defined(_DEBUG)

#define DebugLog(str,...) (Eugene::Debug::GetInstance().Log(str,__VA_ARGS__))

namespace Eugene
{
	/// <summary>
	/// コンソールにデバッグ出力するクラス、シングルトンになっている
	/// </summary>
	class Debug
	{
	public:
		static Debug& GetInstance(void);


#if _MSC_FULL_VER < 193532215 


#if _MSC_FULL_VER <= 193030709
		template<class... Args>
		constexpr void Log(const std::string_view fmt, const Args& ...args)
		{
			Log(std::vformat(fmt, std::make_format_args(args...)));
		}
#else
		/// <summary>
		/// std::formatを使用してフォーマットして出力する
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="fmt"></param>
		/// <param name="...args"></param>
		template<class... Args>
		constexpr void Log(const std::_Fmt_string<Args...>& fmt, const Args ...args)
		{
			Log(std::vformat(fmt._Str, std::make_format_args(args...)));
		}
#endif
#else
		/// <summary>
		/// std::formatを使用してフォーマットして出力する
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="fmt"></param>
		/// <param name="...args"></param>
		template<class... Args>
		constexpr void Log(std::format_string<Args...> fmt, const Args ...args)
		{
			Log(std::vformat(fmt.get(), std::make_format_args(args...)));
		}
#endif

		/// <summary>
		/// デバッグ出力する
		/// </summary>
		/// <param name="str"></param>
		void Log(const std::string& str);

	private:
		Debug();
		~Debug();
		Debug(const Debug&) = delete;
		void operator=(const Debug&) = delete;

		/// <summary>
		/// アクセス制御用バイナリセマフォ
		/// </summary>
		std::binary_semaphore binarySemphore_;

		/// <summary>
		/// スレッドID出力用oss
		/// </summary>
		std::ostringstream oss_;
	};

	
}


#else

#define DebugLog(...) 

#endif

