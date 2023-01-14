#pragma once
#include <string>
#include <string_view>
#include <array>
#include <span>
#include "../Math/Vector2.h"
#include "KeyID.h"
#include "GamePad.h"

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
		/// キーボードの情報()
		/// </summary>
		using KeyDataSpan = std::span<bool, KEYID_MAX>;

		/// <summary>
		/// キーコードのテーブル
		/// </summary>
		using KeyCodeTable = std::array<int, KEYID_MAX>;

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
		virtual bool GetKeyData(KeyDataSpan& keyData) const;

		/// <summary>
		/// キーコードのデータテーブル
		/// </summary>
		/// <param name="keyCodeTable"> セットするキーコード </param>
		/// <returns> 成功時true、失敗時false </returns>
		virtual bool SetKeyCodeTable(KeyCodeTable& keyCodeTable);

		/// <summary>
		/// ゲームパッドを取得する
		/// </summary>
		/// <param name="pad"> ゲームパッド </param>
		/// <param name="idx"></param>
		/// <returns></returns>
		virtual bool GetGamePad(GamePad& pad, std::uint32_t idx) const;
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
