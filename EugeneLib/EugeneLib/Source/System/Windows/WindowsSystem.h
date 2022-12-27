#pragma once
#include "../../../Include/System/System.h"


namespace EugeneLib
{
	class WindowsSystem :
		public System
	{
	public:
		WindowsSystem(const Vector2& size, const std::u8string& title);
		~WindowsSystem();
		Graphics* CreateGraphics(GpuEngine*& gpuEngine, size_t bufferNum) const& final;
	private:
		bool Update(void) final;

	};
}
