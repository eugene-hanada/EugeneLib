﻿#pragma once
#include <string>
#include <string_view>
#include <array>
#include <span>
#include <bitset>
#include <filesystem>
#include "../ThirdParty/glm/glm/vec2.hpp"
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
			/// <summary>
			/// 左クリック
			/// </summary>
			LeftButton,

			/// <summary>
			/// 右クリック
			/// </summary>
			RightButton,

			/// <summary>
			/// ホイール
			/// </summary>
			CenterButton,

			/// <summary>
			/// 追加ボタン1
			/// </summary>
			Other1Button,

			/// <summary>
			/// 追加ボタン2
			/// </summary>
			Other2Button,

			/// <summary>
			/// カーソル表示フラグ
			/// </summary>
			ShowCursor,
			Max
		};

		Mouse();

		/// <summary>
		/// 指定のフラグがtrueかチェックする
		/// </summary>
		/// <param name="flag"> チェックしたいフラグ </param>
		/// <returns> 結果 </returns>
		bool CheckFlags(Flags flag) const;

		/// <summary>
		/// マウス座標
		/// </summary>
		glm::vec2 pos;

		/// <summary>
		/// ホイール
		/// </summary>
		float wheel = 0.0f;

		/// <summary>
		/// フラグ
		/// </summary>
		std::bitset<static_cast<size_t>(Flags::Max)> flags;
	private:
	};

	struct TouchData
	{
	public:
		struct Touch
		{
			Touch();
			glm::vec2 pos;
			float nowTime;
			float downTime;
		};

		TouchData();

		std::uint32_t touchCount_;
		std::array<Touch, 8> touchList_;
	private:
	};

	using UniqueGraphics = std::unique_ptr<Graphics>;
	using UniqueGpuEngine = std::unique_ptr<GpuEngine>;

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
		const glm::vec2& GetWindowSize(void) const&;

		/// <summary>
		/// 最大ウィンドウサイズを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const glm::vec2& GetMaxWindowSize(void) const&;

		/// <summary>
		/// Graphicsを生成する
		/// </summary>
		/// <param name="bufferNum"> バックバッファの数 </param>
		/// <param name="maxSize"> GpuEngineの一度に処理するCommandListの数 </param>
		/// <returns></returns>
		[[nodiscard]]
		virtual std::pair<Graphics*, GpuEngine*> CreateGraphics(std::uint32_t bufferNum = 2, std::uint64_t maxSize = 100) const = 0;

		[[nodiscard]]
		std::pair<UniqueGraphics, UniqueGpuEngine> CreateGraphicsUnique(std::uint32_t bufferNum = 2, std::uint64_t maxSize = 100) const;

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
		/// タッチ状態を取得する
		/// </summary>
		/// <param name="pressed"> タッチした瞬間のもの </param>
		/// <param name="move"> タッチした状態で移動した時のもの </param>
		/// <param name="released"> タッチを話した瞬間のもの </param>
		/// <returns></returns>
		virtual bool GetTouch(TouchData& pressed, TouchData& move, TouchData& released) const;

		/// <summary>
		/// 終了するか？
		/// </summary>
		/// <param name=""> trueの時終了する </param>
		/// <returns></returns>
		virtual bool IsEnd(void) const = 0;

		/// <summary>
		/// 動的リンクライブラリ用のクラスを生成する
		/// </summary>
		/// <param name="path"> ライブラリのパス </param>
		/// <returns> 動的ライブラリを扱うクラスのポインタ </returns>
		[[nodiscard]]
		virtual DynamicLibrary* CreateDynamicLibrary(const std::filesystem::path& path) const = 0;

		/// <summary>
		/// ウィンドウのリサイズを行う
		/// </summary>
		/// <param name="size"></param>
		void ReSizeWindow(const glm::vec2& size);

		/// <summary>
		/// フルスクリーンにしたりする
		/// </summary>
		/// <param name="isFullScreen"> trueでフルスクリーン </param>
		void SetFullScreen(bool isFullScreen);



#ifdef USE_IMGUI

		/// <summary>
		/// Imguiのフレーム開始処理を行う
		/// </summary>
		/// <param name=""></param>
		virtual void ImguiNewFrame(void) const = 0;

		/// <summary>
		/// Systemクラスが作成したImguiのコンテキストを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> Imguiのコンテキスト </returns>
		ImGuiContext* GetContextFromCreatedLib(void) const;
#endif
	protected:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="size"> ウィンドウサイズ </param>
		/// <param name="title"> ウィンドウタイトル </param>
		System(const glm::vec2& size, const std::u8string& title, std::intptr_t other = 0, std::span<std::string_view> directories = {});

		virtual void OnSetFullScreen(bool isFullscreen) = 0;

		virtual void OnResizeWindow(const glm::vec2& size) = 0;

		/// <summary>
		/// ウィンドウサイズ
		/// </summary>
		glm::vec2 windowSize_;

		/// <summary>
		/// タイトル
		/// </summary>
		std::u8string title_;

		/// <summary>
		/// リサイズように使用
		/// </summary>
		static Eugene::Graphics* graphics;

        /// <summary>
        /// リサイズように使用
        /// </summary>
        static GpuEngine* gpuEngine;

		/// <summary>
		/// 最大ウィンドウサイズ
		/// </summary>
		glm::vec2 maxWindowSize_;

#ifdef USE_IMGUI

		/// <summary>
		/// Imgui用コンテキスト
		/// </summary>
		ImGuiContext* context_{ nullptr };
#endif
	private:
		System(const System&) = delete;
		System& operator=(const System&) = delete;

		friend System* CreateSystem(const glm::vec2& size, const std::u8string& title, std::intptr_t other,std::span<std::string_view> directories);
	};

	/// <summary>
	/// Systemの作成
	/// </summary>
	/// <param name="size"> ウィンドウサイズ </param>
	/// <param name="title"> タイトル </param>
	/// <returns></returns>
	[[nodiscard]]
	System* CreateSystem(const glm::vec2& size, const std::u8string& title, std::intptr_t other = 0,std::span<std::string_view> directories = {});

	using UniqueSystem = std::unique_ptr<System>;

	/// <summary>
	/// CreateSystemのstd::unique_ptrを返す版
	/// </summary>
	/// <param name="size"> ウィンドウサイズ </param>
	/// <param name="title"> タイトル </param>
	/// <returns> std::unique_ptrを使用したSystem </returns>
	UniqueSystem CreateSystemUnique(const glm::vec2& size, const std::u8string& title, std::intptr_t other = 0,std::span<std::string_view> directories = {});

	
}
