#pragma once
#include "../ThirdParty/fmt/include/fmt/core.h"
#include <string>
#include <semaphore>
#include <bitset>
#include <sstream>
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
		
		/// <summary>
		/// ログを出力する(フォーマット版)
		/// </summary>
		/// <typeparam name="...Args"> 引数 </typeparam>
		/// <param name="type"></param>
		/// <param name="fmt"> フォーマットの文字列 </param>
		/// <param name="...args"> 引数 </param>
		template<class ...Args>
		void Log(fmt::format_string<Args...> format, const Args ...args)
		{
			Log(fmt::vformat(format, fmt::make_format_args(args...)));
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
		void Error(fmt::format_string<Args...> format, const Args& ...args)
		{
			Error(fmt::vformat(format, fmt::make_format_args(args...)));
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
		void Warning(fmt::format_string<Args...> format, const Args ...args)
		{
			Warning(fmt::vformat(format, fmt::make_format_args(args...)));
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
		void LogDebug(fmt::format_string<Args...> format, const Args ...args)
		{
			LogDebug(fmt::vformat(format.get(), fmt::make_format_args(args...)));
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


		/// <summary>
		/// 出力を行う
		/// </summary>
		/// <param name="type"> 出力タイプ </param>
		/// <param name="string"> 出力する文字列 </param>
		void Out(Type type, const std::string_view& string);


		/// <summary>
		/// アクセス制御用バイナリセマフォ
		/// </summary>
		std::binary_semaphore binarySemphore_;

		/// <summary>
		/// スレッドID出力用oss
		/// </summary>
		std::ostringstream oss_;


		std::stringstream ss_;

		std::ostream& os_;

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


