#include "../../Include/System/System.h"
#include "../../Source/System/Windows/WindowsSystem.h"

Eugene::System* sys = nullptr;

void Eugene::System::GetMouse(Mouse& outMouse) const&
{
}

bool Eugene::System::IsHitKey(KeyID keyID) const
{
	return false;
}

bool Eugene::System::GetKeyData(KeyDataSpan& keyData) const
{
	return false;
}

bool Eugene::System::SetKeyCodeTable(KeyCodeTable& keyCodeTable)
{
	return false;
}

bool Eugene::System::GetGamePad(GamePad& pad, std::uint32_t idx) const
{
	return false;
}

Eugene::System::System(const Eugene::Vector2& size, const std::u8string& title) :
	windowSize_{size}, title_{title}
{
}


Eugene::System::~System()
{
}

const Eugene::Vector2& Eugene::System::GetWindowSize(void) const&
{
	return windowSize_;
}


Eugene::System* Eugene::CreateSystem(const Vector2& size, const std::u8string& title)
{
	if (sys != nullptr)
	{
		return sys;
	}
	return (sys = new WindowsSystem{size,title});
}
