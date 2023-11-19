#pragma once
#include <string>
#include <format>
#include <semaphore>
#include <spanstream>
#include <print>
#include <ostream>
#include <bitset>
#include <sstream>
#include <vector>
#include <filesystem>

#if defined(_DEBUG)
#define DebugIOLog(str,...) (Eugene::Debug::GetInstance().Log(str,__VA_ARGS__))
#define DebugIOLogDebug(str, ...) (Eugene::Debug::GetInstance().LogDebug(str,__VA_ARGS__))
#define DebugIOError(str, ...) (Eugene::Debug::GetInstance().Error(str,__VA_ARGS__))
#define DebugIOWarning(str,...) (Eugene::Debug::GetInstance().Warning(str,__VA_ARGS__))
#else
#define DebugLog(...) 
#endif

#define DebugIO (Eugene::Debug::GetInstance())

namespace Eugene
{
	/// <summary>
	/// コンソールにデバッグ出力するクラス、シングルトンになっている
	/// </summary>
	class Debug
	{
	public:

		/// <summary>
		/// 出力タイプ
		/// </summary>
		enum class Type
		{
			Error,		// エラー
			Warning,	// ワーニング
			Log,		// ログ
			Debug		// デバッグ用ログ
		};

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
		//template<class... Args>
		//constexpr void Log(std::format_string<Args...> fmt, const Args ...args)
		//{
		//	Log(std::vformat(fmt.get(), std::make_format_args(args...)));
		//}
#endif




		

		
		/// <summary>
		/// ログを出力する(フォーマット版)
		/// </summary>
		/// <typeparam name="...Args"> 引数 </typeparam>
		/// <param name="type"></param>
		/// <param name="fmt"> フォーマットの文字列 </param>
		/// <param name="...args"> 引数 </param>
		template<class ...Args>
		void Log(std::format_string<Args...> fmt, const Args ...args)
		{
			Out(Type::Log, fmt, args...);
		}

		/// <summary>
		/// ログを出力する
		/// </summary>
		/// <param name="string"> 文字列 </param>
		void Log(const std::string_view& string);

		/// <summary>
		/// ログにエラー出力する(フォーマット版)
		/// </summary>
		/// <typeparam name="...Args"> 引数 </typeparam>
		/// <param name="type"></param>
		/// <param name="fmt"> フォーマットの文字列 </param>
		/// <param name="...args"> 引数 </param>
		template<class ...Args>
		void Error(std::format_string<Args...> fmt, const Args ...args)
		{
			Out(Type::Error, fmt, args...);
		}

		/// <summary>
		/// ログにエラー出力する
		/// </summary>
		/// <param name="string"> 文字列 </param>
		void Error(const std::string_view& string);

		/// <summary>
		/// ログにワーニング出力する(フォーマット版)
		/// </summary>
		/// <typeparam name="...Args"> 引数 </typeparam>
		/// <param name="type"></param>
		/// <param name="fmt"> フォーマットの文字列 </param>
		/// <param name="...args"> 引数 </param>
		template<class ...Args>
		void Warning(std::format_string<Args...> fmt, const Args ...args)
		{
			Out(Type::Warning, fmt, args...);
		}

		/// <summary>
		/// ログにワーニング出力する
		/// </summary>
		/// <param name="string"> 文字列 </param>
		void Warning(const std::string_view& string);

		/// <summary>
		/// ログにデバッグ用ログを出力する(フォーマット版)
		/// </summary>
		/// <typeparam name="...Args"> 引数 </typeparam>
		/// <param name="type"></param>
		/// <param name="fmt"> フォーマットの文字列 </param>
		/// <param name="...args"> 引数 </param>
		template<class ...Args>
		void LogDebug(std::format_string<Args...> fmt, const Args ...args)
		{
			Out(Type::Debug, fmt, args...);
		}

		/// <summary>
		/// ログにデバッグ用ログを出力する
		/// </summary>
		/// <param name="string"> 文字列 </param>
		void LogDebug(const std::string_view& string);

		/// <summary>
		/// フィルターをクリアする
		/// </summary>
		/// <param name="flag"> クリアする値 </param>
		void ClearFillter(bool flag);

		/// <summary>
		/// フィルターに表示するタイプを追加する
		/// </summary>
		/// <param name="type"> 追加するタイプ </param>
		void AddFilter(Type type);

		/// <summary>
		/// フィルターから表示するタイプを削除する
		/// </summary>
		/// <param name="type"> 削除するタイプ </param>
		void RemoveFilter(Type type);

		/// <summary>
		/// 現在のバッファにある文字列を取得する
		/// </summary>
		/// <returns> 文字列 </returns>
		std::string_view GetBuffer_View(void);

		/// <summary>
		/// バッファをクリアする
		/// </summary>
		/// <param name=""></param>
		void ClearBuffer(void);

		/// <summary>
		/// デバッグ用のコンソールを開く
		/// </summary>
		/// <param name=""></param>
		void OpenConsole(void);

		/// <summary>
		/// ログのファイルを出力するパスをセットする
		/// </summary>
		/// <param name="path"> パス </param>
		void SetExportPath(const std::filesystem::path& path);
	private:
		Debug();
		~Debug();
		Debug(const Debug&) = delete;
		void operator=(const Debug&) = delete;

		/// <summary>
		/// バッファをチェックして必要なら拡張する
		/// </summary>
		/// <param name="string"> 文字列 </param>
		void CheckBuffer(const std::string_view& string) &;

		template<class... Args>
		void Out(Type type, std::format_string<Args...> fmt, const Args ...args)
		{
			Out(type, std::vformat(fmt.get(), std::make_format_args(args...)));
		}

		void Out(Type type, const std::string_view& string);


		/// <summary>
		/// アクセス制御用バイナリセマフォ
		/// </summary>
		std::binary_semaphore binarySemphore_;

		/// <summary>
		/// スレッドID出力用oss
		/// </summary>
		std::ostringstream oss_;

		/// <summary>
		/// 文字列用バッファ
		/// </summary>
		std::vector<char> buffer_;

		/// <summary>
		/// バッファを使用したストリーム
		/// </summary>
		std::spanstream spanStream_;

		/// <summary>
		/// 表示用フィルター
		/// </summary>
		std::bitset<4> filter_;

		/// <summary>
		/// ログをファイルを出力するパス
		/// </summary>
		std::filesystem::path exportPath_;
	};


}

/// <summary>
/// Type用フォーマッター
/// </summary>
template<>
class std::formatter<Eugene::Debug::Type> :
	public std::formatter<const char*>
{
public:
	template<class Out>
	auto format(Eugene::Debug::Type type, std::basic_format_context<Out, char>& ctx)
	{
		return std::formatter<const char*>::format(names[std::to_underlying(type)].data(), ctx);
	}
private:
	static constexpr std::string_view names[]{"\x1B[31;1mERROR\x1B[37;m","\x1B[33;1mWARNING\x1B[37;m","LOG","\x1B[34;1mDEBUG\x1B[37;m"};
};

