#pragma once
#include <string>
#include <string_view>
#include <array>
#include <span>
#include <bitset>
#include <filesystem>
#include "../Math/Vector2.h"
#include "KeyID.h"
#include "GamePad.h"

struct ImGuiContext;

namespace Eugene
{
	class Graphics;
	class GpuEngine;
	class DynamicLibrary;

	/// <summary>
	/// マウスの構造体
	/// </summary>
	struct Mouse
	{
	public:
		/// <summary>
		/// マウス用フラグ
		/// </summary>
		enum class Flags
		{
			LeftButton,
			RightButton,
			CenterButton,
			Other1Button,
			Other2Button,
			ShowCursor,
			Max
		};

		Mouse();
		bool CheckFlags(Flags flag) const;
		Vector2 pos;
		float wheel = 0.0f;
		std::bitset<static_cast<size_t>(Flags::Max)> flags;
	private:
	};

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
		[[nodiscard]]
		virtual Graphics* CreateGraphics(GpuEngine*& gpuEngine, std::uint32_t bufferNum = 2) const& = 0;

		/// <summary>
		/// Graphicsを生成する
		/// </summary>
		/// <param name="bufferNum"> バックバッファの数 </param>
		/// <param name="maxSize"> GpuEngineの一度に処理するCommandListの数 </param>
		/// <returns></returns>
		[[nodiscard]]
		virtual std::pair<Graphics*, GpuEngine*> CreateGraphics(std::uint32_t bufferNum = 2, std::uint64_t maxSize = 100) const = 0;

		/// <summary>
		/// マウスの情報を取得する
		/// </summary>
		/// <param name="outMouse"></param>
		/// <returns></returns>
		virtual bool GetMouse(Mouse& outMouse) const&;

		/// <summary>
		/// マウス情報をセットする
		/// </summary>
		/// <param name="inMouse"></param>
		/// <returns></returns>
		virtual bool SetMouse(Mouse& inMouse) const;

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
		virtual bool GetKeyData(KeyDataSpan keyData) const;

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

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual bool IsEnd(void) const = 0;

		virtual void ResizeWindow(const Vector2& size) = 0;

		/// <summary>
		/// 動的リンクライブラリ用のクラスを生成する
		/// </summary>
		/// <param name="path"> ライブラリのパス </param>
		/// <returns> 動的ライブラリを扱うクラスのポインタ </returns>
		[[nodiscard]]
		virtual DynamicLibrary* CreateDynamicLibrary(const std::filesystem::path& path) const = 0;
#ifdef USE_IMGUI
		virtual void ImguiNewFrame(void) const = 0;
		ImGuiContext* GetContextFromCreatedLib(void) const;
#endif
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

#ifdef USE_IMGUI
		ImGuiContext* context_{ nullptr };
#endif
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
	[[nodiscard]]
	System* CreateSystem(const Vector2& size, const std::u8string& title);
}
