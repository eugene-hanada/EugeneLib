#pragma once
#include <concepts>
#include <string>
#include <filesystem>
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#ifdef _DEBUG

#define DebugLog(...) (Eugene::Debug::GetInstance().LogOut(__VA_ARGS__))


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

		template<class T>
		void LogOut(const T& a)
		{
			Log(ToString(a));
		}

		template<class T1, class T2>
		void LogOut(const T1& a, const T2& b)
		{
			Log(ToString(a) + ToString(b));
		}

		template<class T1, class ...T2>
		void LogOut(const T1& a, const T2&... b)
		{
			Log(ToString() + ToString(b...));
		}



		void Log(const std::u8string& log);

	private:
		Debug();
		~Debug();
		Debug(const Debug&) = delete;
		void operator=(const Debug&) = delete;



		std::u8string ToString(const char8_t a[])
		{
			return a;
		}

		template<DebugValue V>
		std::u8string ToString(const V& v)
		{
			return std::filesystem::path(std::to_string(v)).u8string();
		}

		template<ValueC T>
		std::u8string ToString(const Vector2Tmp<T>& vec)
		{
			return std::filesystem::path("x=" + std::to_string(vec.x) + "y=" + std::to_string(vec.y)).u8string();
		}

		template<ValueC T>
		std::u8string ToString(const Vector3Tmp<T>& vec)
		{
			return std::filesystem::path("x=" + std::to_string(vec.x) + "y=" + std::to_string(vec.y) + std::to_string(vec.z)).u8string();
		}

		template<class A, class B>
		std::u8string ToString(const A& a, const B& b)
		{
			return ToString(a) + ToString(b);
		}

		template<class A, class ...B>
		std::string ToString(const A& a, const B&... b)
		{
			return ToString(a) + ToString(b...);
		}
	};






	
}


#else

#define DebugLog(...) 

#endif
