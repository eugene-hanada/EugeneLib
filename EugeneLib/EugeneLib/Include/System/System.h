#pragma once
#include <string>
#include <string_view>
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
			TAB,
			CAPSLOCK,
			LEFT_SHIFT,
			LEFT_CTRL,
			ESCAPE,
			SPACE,
			Alt,
			BACK_SPACE,
			ENTER,
			RIGHT_SHIFT,
			UP,

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

		virtual void GetMouse(Mouse& outMouse) const&;
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
