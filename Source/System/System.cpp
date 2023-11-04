#include "../../Include/System/System.h"
#include "../../Include/Common/EugeneLibException.h"
#include "../../Include/Graphics/GpuEngine.h"
#include "../../Include/Graphics/Graphics.h"

#ifdef USE_WINDOWS
#include "../../Source/System/Windows/WindowsSystem.h"
#endif

#ifdef USE_IMGUI
#include "../../Include/ThirdParty/imgui/imgui.h"
#endif

namespace
{
	bool isCreate = false;
}

Eugene::Graphics* Eugene::System::graphics{ nullptr };

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

Eugene::System::System(const glm::vec2& size, const std::u8string& title) :
	windowSize_{size}, title_{title}
{
}


Eugene::System::~System()
{
	isCreate = false;
}

const glm::vec2& Eugene::System::GetWindowSize(void) const&
{
	return windowSize_;
}

void Eugene::System::OnResizeWindow(const glm::vec2& size)
{
	// 実装しない
}

void Eugene::System::ReSizeWindow(const glm::vec2& size)
{
	if (windowSize_ == size)
	{
		return;
	}

	windowSize_ = size;
	OnResizeWindow(size);
	if (graphics)
	{
		graphics->ResizeBackBuffer(size);
	}
}

void Eugene::System::SetFullScreen(bool isFullScreen)
{
	OnSetFullScreen(isFullScreen);
}

void Eugene::System::OnSetFullScreen(bool isFullScreen)
{
}


Eugene::System* Eugene::CreateSystem(const glm::vec2& size, const std::u8string& title)
{
	if (isCreate)
	{
		throw CreateErrorException{"Systemはすでに生成しています"};
	}
	isCreate = true;

#ifdef USE_WINDOWS
	return new WindowsSystem{size,title};
#endif
}

Eugene::UniqueSystem Eugene::CreateSystemUnique(const glm::vec2& size, const std::u8string& title)
{
	return UniqueSystem{CreateSystem(size, title)};
}

std::pair<Eugene::UniqueGraphics, Eugene::UniqueGpuEngine> Eugene::System::CreateGraphicsUnique(std::uint32_t bufferNum, std::uint64_t maxSize) const
{
	auto [graphics, gpuEngine] = CreateGraphics(bufferNum, maxSize);
	return std::pair<UniqueGraphics, UniqueGpuEngine>{graphics,gpuEngine};
}

#ifdef USE_IMGUI
ImGuiContext* Eugene::System::GetContextFromCreatedLib(void) const
{
	return context_;
}
#endif