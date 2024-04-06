#pragma once
#include <array>
#include "../../../Include/System/System.h"


namespace Eugene
{
	class System::SystemImpl
	{
	public:
		SystemImpl(System& system,const glm::vec2& size, const std::u8string& title, std::intptr_t other = 0, std::span<std::string_view> directories ={});
		~SystemImpl();
		std::pair<Graphics*, GpuEngine*> CreateGraphics(std::uint32_t bufferNum, std::uint64_t maxSize) const ;
		bool Update(void) ;
		bool GetMouse(Mouse& outMouse) const& ;
		bool SetMouse(Mouse& outMouse) const ;
		bool IsHitKey(KeyID keyID) const ;
		bool GetKeyData(KeyDataSpan keyData) const ;
		bool SetKeyCodeTable(KeyCodeTable& keyCodeTable) ;
		bool GetGamePad(GamePad& pad, std::uint32_t idx) const ;
		bool GetTouch(TouchData& pressed, TouchData& move, TouchData& released) const;
		bool IsEnd(void) const ;
		void OnResizeWindow(const glm::vec2& size) ;
		void OnSetFullScreen(bool isFullScreen) ;
		DynamicLibrary* CreateDynamicLibrary(const std::filesystem::path& path) const ;
#ifdef USE_IMGUI
		void ImguiNewFrame(void) const ;
#endif
	private:
		System& system_;
		/// <summary>
		/// キーコードのテーブル
		/// </summary>
		KeyCodeTable codeTable_{
			9,			// タブ
			240,		// キャプスロック
			160,		// 左シフト
			162,		// 左コントロール
			27,			// エスケープ
			32,			// スペース
			164,
			165,
			8,
			13,
			161,
			163,
			38,
			40,
			37,
			39,
			112,
			113,
			114,
			115,
			116,
			117,
			118,
			119,
			120,
			121,
			122,
			123,
			144,
			96,
			97,
			98,
			99,
			100,
			101,
			102,
			103,
			104,
			105,
			106,
			107,
			109,
			110,
			111,
			48,
			49,
			50,
			51,
			52,
			53,
			54,
			55,
			56,
			57,
			189,
			222,
			220,
			192,
			219,
			187,
			186,
			221,
			188,
			190,
			191,
			226,
			'A',
			'B',
			'C',
			'D',
			'E',
			'F',
			'G',
			'H',
			'I',
			'J',
			'K',
			'L',
			'M',
			'N',
			'O',
			'P',
			'Q',
			'R',
			'S',
			'T',
			'U',
			'V',
			'W',
			'X',
			'Y',
			'Z'
		};
	};
}
