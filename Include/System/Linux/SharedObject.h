﻿#pragma once
#include <filesystem>
#include <string_view>

namespace Eugene {

	class DynamicLibrary
	{
	public:
        DynamicLibrary(const std::filesystem::path& path);
		~DynamicLibrary();


		/// <summary>
		/// 指定された名前の関数のポインタを取得する
		/// </summary>
		/// <typeparam name="func"> 関数の型 </typeparam>
		/// <param name="functionName"> 関数名 </param>
		/// <returns> 関数ポインタ </returns>
		template<typename func>
		func* GetFunction(const std::string_view& functionName)
		{
			return static_cast<func*>(FindFunction(functionName));
		}

	private:
        void* FindFunction(const std::string_view& functionName) const;

		/// <summary>
		/// 共有library
		/// </summary>
		void* dl_;
	};
}
