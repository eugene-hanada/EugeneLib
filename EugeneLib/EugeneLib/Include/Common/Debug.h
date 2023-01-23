#pragma once
#include <concepts>
#include <string>
#include <filesystem>
#include <format>
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#ifdef _DEBUG

#define DebugLog(...) (Eugene::Debug::GetInstance().Log(__VA_ARGS__))

template<>
class std::formatter<Eugene::Vector2>
{
public:
	std::string format(const Eugene::Vector2& vec, std::format_context& fmtCntxt)
	{
		return std::format(std::format("x={0:f}y{1:f}", vec.x, vec.y), fmtCntxt);
	}
};

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
		void Log(const std::string& formatStr, const Args&... args)
		{
			
		}

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

