﻿#include "../../Include/System/System.h"
#include "../../Source/System/Windows/WindowsSystem.h"

#ifdef USE_IMGUI
#include "../../Include/ThirdParty/imgui/imgui.h"
#endif

Eugene::System* sys = nullptr;

Eugene::Mouse::Mouse()
{
	flags.reset();
	flags.set(static_cast<size_t>(Flags::ShowCursor));
}

bool Eugene::Mouse::CheckFlags(Flags flag) const
{
	return flags.test(static_cast<size_t>(flag));
}

bool Eugene::System::GetMouse(Mouse& outMouse) const&
{
	return false;
}

bool Eugene::System::SetMouse(Mouse& inMouse) const
{
	return false;
}

bool Eugene::System::IsHitKey(KeyID keyID) const
{
	return false;
}

bool Eugene::System::GetKeyData(KeyDataSpan keyData) const
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
	sys = nullptr;
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


#ifdef USE_IMGUI
ImGuiContext* Eugene::System::GetContextFromCreatedLib(void) const
{
	return context_;
}
#endif