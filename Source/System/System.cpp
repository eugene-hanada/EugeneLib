#include "../../Include/System/System.h"
#include "../../Source/System/Windows/WindowsSystem.h"
#include "../../Include/Common/EugeneLibException.h"
#include "../../../Include/Graphics/GpuEngine.h"
#include "../../../Include/Graphics/Graphics.h"

#ifdef USE_IMGUI
#include "../../Include/ThirdParty/imgui/imgui.h"
#endif

namespace
{
	bool isCreate = false;
}

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
	isCreate = false;
}

const Eugene::Vector2& Eugene::System::GetWindowSize(void) const&
{
	return windowSize_;
}


Eugene::System* Eugene::CreateSystem(const Vector2& size, const std::u8string& title)
{
	if (isCreate)
	{
		throw CreateErrorException{"すでに生成しています"};
	}
	return new WindowsSystem{size,title};
}

Eugene::UniqueSystem Eugene::CreateSystemUnique(const Vector2& size, const std::u8string& title)
{
	return UniqueSystem{CreateSystem(size, title)};
}

std::pair<std::unique_ptr<Eugene::Graphics>, std::unique_ptr<Eugene::GpuEngine>> Eugene::System::CreateGraphicsUnique(std::uint32_t bufferNum, std::uint64_t maxSize) const
{
	auto [graphics, gpuEngine] = CreateGraphics(bufferNum, maxSize);
	return std::pair<std::unique_ptr<Graphics>, std::unique_ptr<GpuEngine>>{graphics,gpuEngine};
}

#ifdef USE_IMGUI
ImGuiContext* Eugene::System::GetContextFromCreatedLib(void) const
{
	return context_;
}
#endif