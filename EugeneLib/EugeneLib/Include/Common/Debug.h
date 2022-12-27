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

	class Debug
	{
	public:
		static Debug& GetInstance(void);

		void Log(const std::u8string& log);
		void Log(const std::u8string& log1, const std::u8string& log2);
		void Log(const Vector2& vec);
		void Log(const Vector2& vec, const std::u8string& log);
		void Log(float val);
		void Log(float val, const std::u8string& log);


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
