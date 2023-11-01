#pragma once
#include <array>
#include "../../../Include/System/System.h"


namespace Eugene
{
	class WindowsSystem :
		public System
	{
	public:
		WindowsSystem(const glm::vec2& size, const std::u8string& title);
		~WindowsSystem();
		std::pair<Graphics*, GpuEngine*> CreateGraphics(std::uint32_t bufferNum = 2, std::uint64_t maxSize = 100) const final;
	private:
		bool Update(void) final;
		bool GetMouse(Mouse& outMouse) const& final;
		bool SetMouse(Mouse& outMouse) const final;
		bool IsHitKey(KeyID keyID) const final;
		bool GetKeyData(KeyDataSpan keyData) const final;
		bool SetKeyCodeTable(KeyCodeTable& keyCodeTable) final;
		bool GetGamePad(GamePad& pad, std::uint32_t idx) const final;
		bool IsEnd(void) const final;
		void ResizeWindow(const glm::vec2& size) final;
		DynamicLibrary* CreateDynamicLibrary(const std::filesystem::path& path) const final;
#ifdef USE_IMGUI
		void ImguiNewFrame(void) const final;
#endif

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
