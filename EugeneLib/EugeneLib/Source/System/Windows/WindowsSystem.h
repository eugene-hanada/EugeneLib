#pragma once
#include <array>
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
		void GetMouse(Mouse& outMouse) const& final;
		bool IsHitKey(KeyID keyID) const final;
		bool GetKeyData(KeyData& keyData) const final;
		KeyData key_;
	};
}
