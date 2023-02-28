#pragma once
#include <concepts>
#include <string>
#include <filesystem>
#include <format>
#include <semaphore>
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#ifdef _DEBUG

#define DebugLog(...) (Eugene::Debug::GetInstance().Log(__VA_ARGS__))


/// <summary>
/// Vector2用std::formatter
/// </summary>
template<Eugene::ValueC T>
class std::formatter<Eugene::Vector2Tmp<T>> :
	public std::formatter<const char*>
{
public:

	template<class Out>
	auto format(const Eugene::Vector2Tmp<T>& vec, std::basic_format_context<Out, char>& ctx)
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			// 浮動小数点型の時
			return std::format_to(ctx.out(), "x={0:f}y={1:f}", vec.x, vec.y);
		}
		else if constexpr (std::is_integral<T>::value)
		{
			// 整数型の時
			return std::format_to(ctx.out(), "x={0:d}y={1:d}", vec.x, vec.y);
		}

		// それ以外
		return std::format_to(ctx.out(), "x={0:}y={1:}", vec.x, vec.y);
	}
};

/// <summary>
/// Vector3用std::formatter
/// </summary>
/// <typeparam name="CharT"> 文字列の型 </typeparam>
template<Eugene::ValueC T>
class std::formatter<Eugene::Vector3Tmp<T>>
{
public:

	/// <summary>
	/// 
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
			// 浮動小数点型の時
			return std::format_to(ctx.out(), "x={0:f}y={1:f}z={2:f}", vec.x , vec.y, vec.z);
		}
		else if constexpr (std::is_integral<T>::value)
		{
			// 整数型の時
			return std::format_to(ctx.out(), "x={0:d}y={1:d}z={2:d}", vec.x, vec.y, vec.z);
		}

		// それ以外
		return std::format_to(ctx.out(), "x={0:}y={1:}z={2:}", vec.x, vec.y, vec.z);
	}
};

namespace Eugene
{

	class Debug
	{
	public:
		static Debug& GetInstance(void);

		/// <summary>
		/// std::formatを使用して文字列をデバッグ出力する
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="fmt"></param>
		/// <param name="...args"></param>
		template<class... Args>
		constexpr void Log(std::format_string<Args...> fmt, const Args ...args)
		{
			Log(std::vformat(fmt.get(), std::make_format_args(args...)));
		}

		/// <summary>
		/// 文字列をデバッグ出力する
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
		/// スレッドID出力用バッファ
		/// </summary>
		std::vector<char> buff;
	};






	
}


#else

#define DebugLog(...) 

#endif

