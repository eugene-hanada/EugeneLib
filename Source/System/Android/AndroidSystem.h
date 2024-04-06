#pragma once
#include "../../../Include/System/System.h"

struct android_app;

namespace Eugene
{


	class System::SystemImpl
	{
	public:
		SystemImpl(System& system, const glm::vec2& size, const std::u8string& title, std::intptr_t other,std::span<std::string_view> directories);
		~SystemImpl();
		std::pair<Graphics*, GpuEngine*> CreateGraphics(std::uint32_t bufferNum, std::uint64_t maxSize) const;
		bool Update(void);
		bool GetMouse(Mouse& outMouse) const&;
		bool SetMouse(Mouse& outMouse) const;
		bool IsHitKey(KeyID keyID) const;
		bool GetKeyData(KeyDataSpan keyData) const;
		bool SetKeyCodeTable(KeyCodeTable& keyCodeTable);
		bool GetGamePad(GamePad& pad, std::uint32_t idx) const;
        bool GetTouch(TouchData& pressed, TouchData& move, TouchData& released) const;
		bool IsEnd(void) const;
		void OnResizeWindow(const glm::vec2& size);
		void OnSetFullScreen(bool isFullScreen);
		DynamicLibrary* CreateDynamicLibrary(const std::filesystem::path& path) const;
#ifdef USE_IMGUI
		void ImguiNewFrame(void) const;
#endif
	private:

		System& system_;

        android_app* app_;

        bool isEnd_;
	};
}