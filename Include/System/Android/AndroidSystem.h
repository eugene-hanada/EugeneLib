#pragma once
#include "../SystemCommon.h"
#include "../../Utils/Utils.h"
#include "../../ThirdParty/glm/glm/vec2.hpp"
#include <string>
#include <span>
#include <atomic>



struct android_app;

namespace Eugene
{


    class System
    {
    public:
        /// <summary>
        /// 生成する
        /// </summary>
        /// <param name="size"> ウィンドウサイズ </param>
        /// <param name="title"> タイトル </param>
        /// <param name="other"> そのほかのデータ(プラットフォームによっては使用する) </param>
        /// <param name="directories"> コピーを行うディレクトリ(プラットフォームによっては使用する) </param>
        static void Create(const glm::vec2& size, const std::u8string& title, std::intptr_t other = 0, std::span<std::string_view> directories = {})
        {
            DynamicSingleton::Create(size, title, other, directories);
        }
        ~System();

        /// <summary>
        /// 更新処理
        /// </summary>
        /// <returns> 継続しない場合false </returns>
        bool Update(void);

        /// <summary>
        /// マウス情報を取得する
        /// </summary>
        /// <param name="outMouse"></param>
        /// <returns></returns>
        bool GetMouse(Mouse& outMouse) const&;

        /// <summary>
        /// マウス情報をセットする
        /// </summary>
        /// <param name="outMouse"></param>
        /// <returns></returns>
        bool SetMouse(Mouse& outMouse) const;

        /// <summary>
        /// キー入力されたか？
        /// </summary>
        /// <param name="keyID"></param>
        /// <returns></returns>
        bool IsHitKey(KeyID keyID) const;

        /// <summary>
        /// キーボードデータを取得する
        /// </summary>
        /// <param name="keyData"></param>
        /// <returns></returns>
        bool GetKeyData(KeyDataSpan keyData) const;

        /// <summary>
        /// キーボードデータをセットする
        /// </summary>
        /// <param name="keyCodeTable"></param>
        /// <returns></returns>
        bool SetKeyCodeTable(KeyCodeTable& keyCodeTable);

        /// <summary>
        /// ゲームパッド情報を取得する
        /// </summary>
        /// <param name="pad"></param>
        /// <param name="idx"></param>
        /// <returns></returns>
        bool GetGamePad(GamePad& pad, std::uint32_t idx) const;


        /// <summary>
        /// タッチ情報を取得する
        /// </summary>
        /// <param name="pressed"></param>
        /// <param name="move"></param>
        /// <param name="released"></param>
        /// <returns></returns>
        bool GetTouch(TouchData& pressed, TouchData& move, TouchData& released) const;

        /// <summary>
        /// 終了するか？
        /// </summary>
        /// <param name=""></param>
        /// <returns></returns>
        bool IsEnd(void) const;

        /// <summary>
        /// ウィンドウサイズをリサイズする
        /// </summary>
        /// <param name="size"></param>
        void ResizeWindow(const glm::vec2& size);

        /// <summary>
        /// フルスクリーン状態をセットする
        /// </summary>
        /// <param name="isFullScreen"> trueでフルスクリーン </param>
        void SetFullScreen(bool isFullScreen);

        /// <summary>
        /// アクティブフラグか？
        /// </summary>
        /// <returns></returns>
        bool IsActive() const noexcept
        {
            return isActive_.load();
        }

        /// <summary>
        /// ウィンドウサイズを取得する
        /// </summary>
        /// <param name=""></param>
        /// <returns> ウィンドウサイズ </returns>
        const glm::vec2& GetWindowSize(void) const& noexcept
        {
            return windowSize_;
        }

        /// <summary>
        /// 最大ウィンドウサイズを取得する
        /// </summary>
        /// <param name=""></param>
        /// <returns></returns>
        const glm::vec2& GetMaxWindowSize(void) const& noexcept
        {
            return maxWindowSize_;
        }

        /// <summary>
        /// API側のウィンドウを取得する
        /// </summary>
        /// <returns> ウィンドウのポインタ </returns>
        void* GetWindow() const noexcept
        {
            return app_;
        }

#ifdef USE_IMGUI

        /// <summary>
        /// IMGUIでフレーム開始処理を行う
        /// </summary>
        /// <param name=""></param>
        void ImguiNewFrame(void) const;
#endif
    private:

        System(const glm::vec2& size, const std::u8string& title, std::intptr_t other = 0, std::span<std::string_view> directories = {});

        /// <summary>
        /// ウィンドウサイズ
        /// </summary>
        glm::vec2 windowSize_;

        /// <summary>
        /// 最大ウィンドウサイズ
        /// </summary>
        glm::vec2 maxWindowSize_;

        std::atomic_bool isActive_;

        android_app* app_;

        bool isEnd_;
    };
}