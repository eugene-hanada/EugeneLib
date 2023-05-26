#pragma once
#include <filesystem>
#include <string>

namespace Eugene
{

	/// <summary>
	/// DLLとかの動的リンクするライブラリを扱うためのクラス
	/// </summary>
	class DynamicLibrary
	{
	public:
		virtual ~DynamicLibrary();

		/// <summary>
		/// 指定された名前の関数のポインタを取得する
		/// </summary>
		/// <typeparam name="func"> 関数の型 </typeparam>
		/// <param name="functionName"> 関数名 </param>
		/// <returns> 関数ポインタ </returns>
		template<typename func>
		func* GetFunction(const std::string& functionName)
		{
			return static_cast<func*>(FindFunction(functionName));
		}

	protected:
		DynamicLibrary();

		/// <summary>
		/// 関数名の関数を取得する
		/// </summary>
		/// <param name="functionName"></param>
		/// <returns></returns>
		virtual void* FindFunction(const std::string& functionName) const = 0;
	};

}