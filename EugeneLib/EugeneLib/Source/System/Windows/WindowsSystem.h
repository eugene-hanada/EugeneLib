#pragma once
#include <array>
#include "../../../Include/System/System.h"


namespace EugeneLib
{
	class WindowsSystem :
		public System
	{
	public:
		WindowsSystem(const Vector2& size, const std::u8string& title);
		~WindowsSystem();
		Graphics* CreateGraphics(GpuEngine*& gpuEngine, size_t bufferNum) const& final;
	private:
		bool Update(void) final;
		void GetMouse(Mouse& outMouse) const& final;
		bool IsHitKey(KeyID keyID) const final;
		bool GetKeyData(KeyData& keyData) const final;
		bool SetKeyCodeTable(KeyCodeTable& keyCodeTable) final;

		/// <summary>
		/// キー情報
		/// </summary>
		KeyData key_;

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
