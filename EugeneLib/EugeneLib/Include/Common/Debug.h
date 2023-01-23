#pragma once
#include <concepts>
#include <string>
#include <filesystem>
#include <format>
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#ifdef _DEBUG

#define DebugLog(...) (Eugene::Debug::GetInstance().Log(__VA_ARGS__))

namespace Eugene
{
	template<class T>
	concept DebugStr = std::same_as<T, std::u8string>;

	template<class T>
	concept DebugValue = std::floating_point<T> || std::signed_integral<T> || std::unsigned_integral<T>;


	class Debug
	{
	public:
		static Debug& GetInstance(void);

		template<class ...Args>
		constexpr void Log(const std::string& formatStr, const Args&... args)
		{
			Log(std::format(formatStr, args));
		}

		void Log(const std::string& str);

	private:
		Debug();
		~Debug();
		Debug(const Debug&) = delete;
		void operator=(const Debug&) = delete;


	};






	
}


#else

#define DebugLog(...) 

#endif

