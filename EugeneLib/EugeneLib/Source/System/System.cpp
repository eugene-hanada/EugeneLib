#include "../../Include/System/System.h"
#include "../../Source/System/Windows/WindowsSystem.h"

EugeneLib::System* sys = nullptr;

void EugeneLib::System::GetMouse(Mouse& outMouse) const&
{
}

bool EugeneLib::System::IsHitKey(KeyID keyID) const
{
	return false;
}

bool EugeneLib::System::GetKeyData(KeyDataSpan& keyData) const
{
	return false;
}

bool EugeneLib::System::SetKeyCodeTable(KeyCodeTable& keyCodeTable)
{
	return false;
}

bool EugeneLib::System::GetGamePad(GamePad& pad, std::uint32_t idx) const
{
	return false;
}

EugeneLib::System::System(const EugeneLib::Vector2& size, const std::u8string& title) :
	windowSize_{size}, title_{title}
{
}


EugeneLib::System::~System()
{
}

const EugeneLib::Vector2& EugeneLib::System::GetWindowSize(void) const&
{
	return windowSize_;
}


EugeneLib::System* EugeneLib::CreateSystem(const Vector2& size, const std::u8string& title)
{
	if (sys != nullptr)
	{
		return sys;
	}
	return (sys = new WindowsSystem{size,title});
}
