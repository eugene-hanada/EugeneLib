#pragma once
#include <string>
#include "Vector2.h"

namespace EugeneLib
{
	class System
	{
	public:
		static bool Init(const Vector2& wsize,const std::u8string& title);
		static bool End();
		static System* GetInstance(void)
		{
			return instance_;
		}
		bool Update(void);
	private:
		static System* instance_;
	};
};