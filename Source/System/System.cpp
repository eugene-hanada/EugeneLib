#include "../../Include/System/System.h"
#include "../../Include/Common/EugeneLibException.h"
#include "../../Include/Graphics/GpuEngine.h"
#include "../../Include/Graphics/Graphics.h"

#ifdef USE_WINDOWS
#include "../../Source/System/Windows/WindowsSystem.h"
#elif USE_ANDROID
#include "../../Source/System/Android/AndroidSystem.h"
#endif

#ifdef USE_IMGUI
#include "../../Include/ThirdParty/imgui/imgui.h"
#endif

namespace
{
	bool isCreate = false;
}

Eugene::Graphics* Eugene::System::graphics{ nullptr };
Eugene::GpuEngine* Eugene::System::gpuEngine{nullptr};

Eugene::Mouse::Mouse() :
	pos{0.0f,0.0f}
{
	flags.reset();
	flags.set(static_cast<size_t>(Flags::ShowCursor));
}

bool Eugene::Mouse::CheckFlags(Flags flag) const
{
	return flags.test(static_cast<size_t>(flag));
}

Eugene::TouchData::Touch::Touch() :
	pos{0.0f,0.0f}, nowTime{0.0f},downTime{0.0f}
{
}

Eugene::TouchData::TouchData() :
	touchCount_{0u}
{
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

bool Eugene::System::GetTouch(TouchData& pressed, TouchData& move, TouchData& released) const
{
	return false;
}

bool Eugene::System::IsEnd(void) const
{
	return false;
}


Eugene::System::System(const glm::vec2& size, const std::u8string& title, std::intptr_t other,std::span<std::string_view> directories) :
	windowSize_{size}, title_{title}
{
	//impl_ = std::make_unique<SystemImpl>(*this,size, title_, other,directories);
}

//bool Eugene::System::Update(void)
//{
//	return impl_->Update();
//}

Eugene::System::~System()
{
	isCreate = false;
}

const glm::vec2& Eugene::System::GetWindowSize(void) const&
{
	return windowSize_;
}

const glm::vec2& Eugene::System::GetMaxWindowSize(void) const&
{
	return maxWindowSize_;
}

std::pair<Eugene::Graphics*, Eugene::GpuEngine*> Eugene::System::CreateGraphics(std::uint32_t bufferNum, std::uint64_t maxSize) const
{
	return CreateGraphics(bufferNum, maxSize);
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
	if (graphics)
	{
		graphics->SetFullScreenFlag(isFullScreen);
	}
}



Eugene::System* Eugene::CreateSystem(const glm::vec2& size, const std::u8string& title, std::intptr_t other,std::span<std::string_view> directories)
{
	if (isCreate)
	{
		throw CreateErrorException{"Systemはすでに生成しています"};
	}
	isCreate = true;

#ifdef USE_WINDOWS
	return new WindowsSystem{size,title,other,directories};
#elif USE_ANDROID
	return new AndroidSystem{ size,title,other,directories };
#endif 
}

Eugene::UniqueSystem Eugene::CreateSystemUnique(const glm::vec2& size, const std::u8string& title, std::intptr_t other,std::span<std::string_view> directories)
{
	return UniqueSystem{CreateSystem(size, title,other,directories)};
}

std::pair<Eugene::UniqueGraphics, Eugene::UniqueGpuEngine> Eugene::System::CreateGraphicsUnique(std::uint32_t bufferNum, std::uint64_t maxSize) const
{
	auto [graphics, gpuEngine] = CreateGraphics(bufferNum, maxSize);
	return std::pair<UniqueGraphics, UniqueGpuEngine>{graphics,gpuEngine};
}



//#ifdef USE_IMGUI
//void Eugene::System::ImguiNewFrame(void) const
//{
//	impl_->ImguiNewFrame();
//}
//
//ImGuiContext* Eugene::System::GetContextFromCreatedLib(void) const
//{
//	return context_;
//}
//#endif