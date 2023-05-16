#pragma once
#include <concepts>
#include <string>
#include <filesystem>
#include <format>
#include <semaphore>
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#ifdef _DEBUG

#define DebugLog(str,...) (Eugene::Debug::GetInstance().Log(str,__VA_ARGS__))


/// <summary>
/// Vector2用std::formatter
/// </summary>
template<Eugene::ValueC T>
class std::formatter<Eugene::Vector2Tmp<T>> :
	public std::formatter<const char*>
{
public:

	/// <summary>
	/// Vector2をstd::formatで出力する
	/// </summary>
	/// <typeparam name="Out"></typeparam>
	/// <param name="vec"></param>
	/// <param name="ctx"></param>
	/// <returns></returns>
	template<class Out>
	auto format(const Eugene::Vector2Tmp<T>& vec, std::basic_format_context<Out, char>& ctx)
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			// 浮動小数点数の時
			return std::format_to(ctx.out(), "x={0:f}y={1:f}", vec.x, vec.y);
		}
		else if constexpr (std::is_integral<T>::value)
		{
			// 整数値の時
			return std::format_to(ctx.out(), "x={0:d}y={1:d}", vec.x, vec.y);
		}

		// それ以外
		return std::format_to(ctx.out(), "x={0:}y={1:}", vec.x, vec.y);
	}
};

/// <summary>
/// Vector3用std::formatter
/// </summary>
/// <typeparam name="CharT">  </typeparam>
template<Eugene::ValueC T>
class std::formatter<Eugene::Vector3Tmp<T>> :
	public std::formatter<const char*>
{
public:

	/// <summary>
	/// Vector3をstd::formatで出力する
	/// </summary>
	/// <typeparam name="Out"></typeparam>
	/// <param name="vec"></param>
	/// <param name="ctx"></param>
	/// <returns></returns>
	template<class Out>
	auto format(const Eugene::Vector3Tmp<T>& vec, std::basic_format_context<Out, char>& ctx)
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			// ���������_�^�̎�
			return std::format_to(ctx.out(), "x={0:f}y={1:f}z={2:f}", vec.x , vec.y, vec.z);
		}
		else if constexpr (std::is_integral<T>::value)
		{
			// �����^�̎�
			return std::format_to(ctx.out(), "x={0:d}y={1:d}z={2:d}", vec.x, vec.y, vec.z);
		}

		// ����ȊO
		return std::format_to(ctx.out(), "x={0:}y={1:}z={2:}", vec.x, vec.y, vec.z);
	}
};

namespace Eugene
{

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

