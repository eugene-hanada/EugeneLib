#pragma once
#include <string>
#include <string_view>
#include "../Math/Vector2.h"

namespace EugeneLib
{
	class System
	{
	public:
		virtual bool Update(void) = 0;
		virtual ~System();
		const Vector2& GetWindowSize(void) const&;
		virtual void* GetWindowHandle(void) const;
	protected:
		System(const Vector2& size, const std::u8string& title);
		
		
		Vector2 windowSize_;
		std::u8string title_;
	private:
		
		System(const System&) = delete;
		System& operator=(const System&) = delete;

	};

	System* CreateSystem(const Vector2& size, const std::u8string& title);
}
