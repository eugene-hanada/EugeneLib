#pragma once
#include <concepts>
#include <string>
#include "../Math/Vector2.h"
#ifdef _DEBUG

#define DebugLog(...) (EugeneLib::Debug::GetInstance().Log(__VA_ARGS__))

namespace EugeneLib
{
	template<class T>
	concept DebugStr = std::same_as<T, std::u8string>;

	template<class T>
	concept DebugValue = std::floating_point<T> || std::signed_integral<T> || std::unsigned_integral<T>;

	class Debug
	{
	public:
		static Debug& GetInstance(void);
		template<class T1, class ...T2>
		void Log(const T1& a, const T2&... b)
		{
			Log(a, ...b);
		}

		void Log(const std::u8string& log);
		void Log(const std::u8string& log1, const std::u8string& log2);
		void Log(const Vector2& vec);
		void Log(const Vector2& vec, const std::u8string& log);
		void Log(float val);
		void Log(float val, const std::u8string& log);
		void Log(std::int32_t val);
		void Log(std::int32_t val, const std::u8string& log);

	private:
		Debug();
		~Debug();
		Debug(const Debug&) = delete;
		void operator=(const Debug&) = delete;
	};

	


	template<DebugValue v>
	std::u8string operator,(const std::u8string& l, v val)
	{
		return l + std::to_string(val);
	}

	template<DebugValue v>
	std::u8string operator,(v val,const std::u8string& r)
	{
		return std::to_string(val) + r;
	}
}

#else

#define DebugLog(...) 

#endif
