#pragma once
#include "../SystemCommon.h"
#include "../../Utils/Utils.h"
#include "../../ThirdParty/glm/glm/vec2.hpp"
#include <string>
#include <span>
#include <atomic>



struct android_app;
struct GameActivityMotionEvent;
struct GameActivity;
struct ANativeWindow;

namespace Eugene
{
    struct Mouse;
    struct TouchData;
    struct GamePad;
    class System :
            public DynamicSingleton<System>
    {
    public:
        /// <summary>
        /// ��������
        /// </summary>
        /// <param name="size"> �E�B���h�E�T�C�Y </param>
        /// <param name="title"> �^�C�g�� </param>
        /// <param name="other"> ���̂ق��̃f�[�^(�v���b�g�t�H�[���ɂ���Ă͎g�p����) </param>
        /// <param name="directories"> �R�s�[���s���f�B���N�g��(�v���b�g�t�H�[���ɂ���Ă͎g�p����) </param>
        static void Create(const glm::vec2& size, const std::u8string& title, std::intptr_t other = 0, std::span<std::string_view> directories = {})
        {
            DynamicSingleton::Create(size, title, other, directories);
        }
        ~System();

        /// <summary>
        /// �X�V����
        /// </summary>
        /// <returns> �p�����Ȃ��ꍇfalse </returns>
        bool Update(void);

        /// <summary>
        /// �}�E�X�����擾����
        /// </summary>
        /// <param name="outMouse"></param>
        /// <returns></returns>
        bool GetMouse(Mouse& outMouse) const&;

        /// <summary>
        /// �}�E�X�����Z�b�g����
        /// </summary>
        /// <param name="outMouse"></param>
        /// <returns></returns>
        bool SetMouse(Mouse& outMouse) const;

        /// <summary>
        /// �L�[���͂��ꂽ���H
        /// </summary>
        /// <param name="keyID"></param>
        /// <returns></returns>
        bool IsHitKey(KeyID keyID) const;

        /// <summary>
        /// �L�[�{�[�h�f�[�^���擾����
        /// </summary>
        /// <param name="keyData"></param>
        /// <returns></returns>
        bool GetKeyData(KeyDataSpan keyData) const;

        /// <summary>
        /// �L�[�{�[�h�f�[�^���Z�b�g����
        /// </summary>
        /// <param name="keyCodeTable"></param>
        /// <returns></returns>
        bool SetKeyCodeTable(KeyCodeTable& keyCodeTable);

        /// <summary>
        /// �Q�[���p�b�h�����擾����
        /// </summary>
        /// <param name="pad"></param>
        /// <param name="idx"></param>
        /// <returns></returns>
        bool GetGamePad(GamePad& pad, std::uint32_t idx) const;


        /// <summary>
        /// �^�b�`�����擾����
        /// </summary>
        /// <param name="pressed"></param>
        /// <param name="move"></param>
        /// <param name="released"></param>
        /// <returns></returns>
        bool GetTouch(TouchData& pressed, TouchData& move, TouchData& released) const;

        /// <summary>
        /// �I�����邩�H
        /// </summary>
        /// <param name=""></param>
        /// <returns></returns>
        bool IsEnd(void) const;

        /// <summary>
        /// �E�B���h�E�T�C�Y�����T�C�Y����
        /// </summary>
        /// <param name="size"></param>
        void ResizeWindow(const glm::vec2& size);

        /// <summary>
        /// �t���X�N���[����Ԃ��Z�b�g����
        /// </summary>
        /// <param name="isFullScreen"> true�Ńt���X�N���[�� </param>
        void SetFullScreen(bool isFullScreen);

        /// <summary>
        /// �A�N�e�B�u�t���O���H
        /// </summary>
        /// <returns></returns>
        bool IsActive() const noexcept
        {
            return isActive_.load();
        }

        /// <summary>
        /// �E�B���h�E�T�C�Y���擾����
        /// </summary>
        /// <param name=""></param>
        /// <returns> �E�B���h�E�T�C�Y </returns>
        const glm::vec2& GetWindowSize(void) const& noexcept
        {
            return windowSize_;
        }

        /// <summary>
        /// �ő�E�B���h�E�T�C�Y���擾����
        /// </summary>
        /// <param name=""></param>
        /// <returns></returns>
        const glm::vec2& GetMaxWindowSize(void) const& noexcept
        {
            return maxWindowSize_;
        }

        /// <summary>
        /// API���̃E�B���h�E���擾����
        /// </summary>
        /// <returns> �E�B���h�E�̃|�C���^ </returns>
        void* GetWindow() const noexcept
        {
            return app_;
        }

#ifdef EUGENE_IMGUI

        /// <summary>
        /// IMGUI�Ńt���[���J�n�������s��
        /// </summary>
        /// <param name=""></param>
        void ImguiNewFrame(void) const;
#endif
    private:

        System(const glm::vec2& size, const std::u8string& title, std::intptr_t other = 0, std::span<std::string_view> directories = {});

        static void OnHandleCmd(android_app *pApp, std::int32_t cmd);

        static void OnPause(GameActivity* activity);

        static void OnResume(GameActivity* activity);

        static void OnChangeFocus(GameActivity* activity,bool flag);

        static void OnWindowCreated(GameActivity* activity,ANativeWindow* window);

        static bool OnMotionEventFilter(const GameActivityMotionEvent* motionEvent);

        /// <summary>
        /// �E�B���h�E�T�C�Y
        /// </summary>
        glm::vec2 windowSize_;

        /// <summary>
        /// �ő�E�B���h�E�T�C�Y
        /// </summary>
        glm::vec2 maxWindowSize_;

        std::atomic_bool isActive_;

        android_app* app_;

        bool isEnd_;
        friend class DynamicSingleton<System>;
    };
}
