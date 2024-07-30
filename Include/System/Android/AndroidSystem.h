#pragma once
#include "../../../Include/System/System.h"

struct android_app;

namespace Eugene
{


	class AndroidSystem:
		public System
	{
	public:
		AndroidSystem(const glm::vec2& size, const std::u8string& title, std::intptr_t other,std::span<std::string_view> directories);
		~AndroidSystem();
		std::pair<Graphics*, GpuEngine*> CreateGraphics(std::uint32_t bufferNum, std::uint64_t maxSize) const final;
		bool Update(void) final;
		bool GetMouse(Mouse& outMouse) const& final;
		bool SetMouse(Mouse& outMouse) const final;
		bool IsHitKey(KeyID keyID) const final;
		bool GetKeyData(KeyDataSpan keyData) const final;
		bool SetKeyCodeTable(KeyCodeTable& keyCodeTable) final;
		bool GetGamePad(GamePad& pad, std::uint32_t idx) const final;
        bool GetTouch(TouchData& pressed, TouchData& move, TouchData& released) const final;
		bool IsEnd(void) const final;
		void OnResizeWindow(const glm::vec2& size) final;
		void OnSetFullScreen(bool isFullScreen) final;
		DynamicLibrary* CreateDynamicLibrary(const std::filesystem::path& path) const final;
#ifdef USE_IMGUI
		void ImguiNewFrame(void) const final;
#endif
	private:

        android_app* app_;

        bool isEnd_;
	};
}