#pragma once
#include <string>
#include <string_view>
#include <array>
#include "../Math/Vector2.h"

namespace EugeneLib
{
	class Graphics;
	class GpuEngine;

	/// <summary>
	/// OS等の処理をするクラス
	/// </summary>
	class System
	{
	public:
		using KeyData = std::array<bool, 256>;

		/// <summary>
		/// マウスの構造体
		/// </summary>
		struct Mouse
		{
			Vector2 pos;
			bool left = false;
			bool right = false;
			float wheel = 0.0f;
		};

		/// <summary>
		/// キーボードのID
		/// </summary>
		enum class KeyID
		{
			TAB = 9,
			CAPS_LOCK = 240,
			LEFT_SHIFT = 160,
			LEFT_CTRL = 162,
			ESCAPE = 27,
			SPACE = 32,
			LEFT_ALT = 164,
			RIGHT_ALT = 165,
			BACK_SPACE = 8,
			RETURN = 13,
			RIGHT_SHIFT = 161,
			RIGHT_CTRL = 163,
			UP = 38,
			DOWN = 40,
			LEFT = 37,
			RIGHT = 39,
			NUM_LOCK = 144,
			F1 = 112,
			F2 = 113,
			F3 = 114,
			F4 = 115,
			F5 = 116,
			F6 = 117,
			F7 = 118,
			F8 = 119,
			F9 = 120,
			F10 = 121,
			F11 = 122,
			F12 = 123,
			NUMPAD_0 = 96,
			NUMPAD_1 = 97,
			NUMPAD_2 = 98,
			NUMPAD_3 = 99,
			NUMPAD_4 = 100,
			NUMPAD_5 = 101,
			NUMPAD_6 = 102,
			NUMPAD_7 = 103,
			NUMPAD_8 = 104,
			NUMPAD_9 = 105,
			A = 'A',
			B = 'B',
			C = 'C',
			D = 'D',
			E = 'E',
			F = 'F',
			G = 'G',
			H = 'H',
			I = 'I',
			J = 'J',
			K = 'K',
			L = 'L',
			M = 'M',
			N = 'N',
			O = 'O',
			P = 'P',
			Q = 'Q',
			R = 'R',
			S = 'S',
			T = 'T',
			U = 'U',
			V = 'V',
			W = 'W',
			X = 'X',
			Y = 'Y',
			Z = 'Z',
		};

		/// <summary>
		/// 更新処理(毎フレーム呼ぶ必要があり返値で終了すべきかが帰ってくる)
		/// </summary>
		/// <param name=""></param>
		/// <returns> 継続すべき時true、終了すべき時false </returns>
		virtual bool Update(void) = 0;

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~System();

		/// <summary>
		/// ウィンドウサイズを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> ウィンドウサイズ </returns>
		const Vector2& GetWindowSize(void) const&;

		/// <summary>
		/// Graphicsをを生成する
		/// </summary>
		/// <param name="gpuEngine"> GpuEngineのポインタの参照 </param>
		/// <returns> Graphicsのポインタ </returns>
		virtual Graphics* CreateGraphics(GpuEngine*& gpuEngine, size_t bufferNum = 2) const& = 0;

		/// <summary>
		/// マウスの情報を取得する
		/// </summary>
		/// <param name="outMouse"></param>
		/// <returns></returns>
		virtual void GetMouse(Mouse& outMouse) const&;

		/// <summary>
		/// キーが押されているか
		/// </summary>
		/// <param name="keyID"> 押されているキーのID </param>
		/// <returns> 押されているときtrue、押されていないときfalse </returns>
		virtual bool IsHitKey(KeyID keyID) const;

		/// <summary>
		/// キー入力情報を取得する
		/// </summary>
		/// <param name="keySpan"> キー情報 </param>
		/// <returns> キー情報がある場合はtrue、ない場合はfalse </returns>
		virtual bool GetKeyData(KeyData& keyData) const;
	protected:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="size"> ウィンドウサイズ </param>
		/// <param name="title"> ウィンドウタイトル </param>
		System(const Vector2& size, const std::u8string& title);
		
		/// <summary>
		/// ウィンドウサイズ
		/// </summary>
		Vector2 windowSize_;

		/// <summary>
		/// タイトル
		/// </summary>
		std::u8string title_;
	private:
		System(const System&) = delete;
		System& operator=(const System&) = delete;

	};

	/// <summary>
	/// Systemの作成
	/// </summary>
	/// <param name="size"> ウィンドウサイズ </param>
	/// <param name="title"> タイトル </param>
	/// <returns></returns>
	System* CreateSystem(const Vector2& size, const std::u8string& title);
}
