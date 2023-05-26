#pragma once
#include "../../../Include/System/DynamicLibrary.h"
#include <Windows.h>

namespace Eugene {

	class Dll :
		public DynamicLibrary
	{
	public:
		Dll(const std::filesystem::path& path);
		~Dll();
	private:
		void* FindFunction(const std::string& functionName) const final;

		/// <summary>
		/// DLLのハンドル
		/// </summary>
		HMODULE handle_;
	};
}
