#pragma once
#include <Windows.h>
#include <string>
#include "Vector2.h"

namespace EugeneLib
{
	
	class System
	{
	public:
		static bool Init(const Vector2& wsize,const std::u8string& title);
		static bool End();

		static System* Ptr(void)
		{
			return instance_;
		}
		bool Update(void);
	private:
		System(const Vector2& wsize, const std::u8string& title);

		System(const System&) = delete;
		System& operator=(const System&) = delete;

		static System* instance_;
		Vector2 v;
	};
}