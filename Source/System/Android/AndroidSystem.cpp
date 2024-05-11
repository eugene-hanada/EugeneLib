#include "AndroidSystem.h"
#include <functional>
#include <fstream>
#include <vector>
#include "../../Graphics/Vulkan/VkGraphics.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "../../../Include/Debug/Debug.h"

namespace
{
    std::int32_t  command{0};

    std::function<void(void)> onPause;
    std::function<void(ANativeWindow*)> onResume;
    std::function<void(android_app*,int,int)> reSize;
    bool isResized = false;
    void OnHandleCmd(android_app *pApp, int32_t cmd)
    {
        switch (cmd)
        {
            case APP_CMD_INIT_WINDOW:
                // A new window is created, associate a renderer with it. You may replace this with a
                // "game" class if that suits your needs. Remember to change all instances of userData
                // if you change the class here as a reinterpret_cast is dangerous this in the
                // android_main function and the APP_CMD_TERM_WINDOW handler case
                DebugIO.Log("InitWindow");
                break;
            case APP_CMD_TERM_WINDOW:
                DebugIO.Log("TermWindow");
                break;
            case APP_CMD_WINDOW_RESIZED:
                DebugIO.Log("WindowResized x={}y={}",ANativeWindow_getWidth(pApp->window),ANativeWindow_getHeight(pApp->window));
                reSize(pApp,ANativeWindow_getWidth(pApp->window),ANativeWindow_getHeight(pApp->window));
                break;
            case APP_CMD_LOST_FOCUS:
                DebugIO.Log("LostFocus");
                break;
            case APP_CMD_PAUSE:

                DebugIO.Log("Pause");
                break;
            case APP_CMD_GAINED_FOCUS:
                DebugIO.Log("GainedFocus");
                break;
            default:
                break;
        }
        command = cmd;
        DebugIO.Log("Cmd={}",command);
    }

    bool OnMotionEventFilter(const GameActivityMotionEvent* motionEvent)
    {
        auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
        return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
                sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
    }

    bool OnKeyEventFilter(const GameActivityKeyEvent* event)
    {
        DebugIO.Log("KeyEvent");
        return true;
    }
}

Eugene::AndroidSystem::AndroidSystem(const glm::vec2& size, const std::u8string& title, std::intptr_t other,std::span<std::string_view> directories) :
	System{size,title},app_{reinterpret_cast<android_app*>(other)},isEnd_{false}
{
    app_->onAppCmd = OnHandleCmd;
    app_->userData = this;
    DebugIO.Log("internalDataPath={}",app_->activity->internalDataPath);

    AAssetDir* dir{nullptr};
    for (const auto& directory:directories)
    {
        if ((dir = AAssetManager_openDir(app_->activity->assetManager,directory.data())) != nullptr)
        {
            AAsset* asset{nullptr};
            for (auto fileName = AAssetDir_getNextFileName(dir); fileName != nullptr; fileName = AAssetDir_getNextFileName(dir))
            {
                std::string path;
                if (directory.size() == 0)
                {
                    path = fmt::format("{}",fileName);
                }
                else
                {
                    std::filesystem::create_directories(fmt::format("{0:}/{1:}",app_->activity->internalDataPath,directory));
                    path = fmt::format("{0:}/{1:}", directory, fileName);
                }
                asset = AAssetManager_open(app_->activity->assetManager, path.c_str(), AASSET_MODE_BUFFER);
                if (asset == nullptr)
                {
                    continue;
                }

                std::ofstream file{
                        fmt::format("{0:}/{1:}", app_->activity->internalDataPath, path),
                        std::ios::binary | std::ios::out
                };

                if (!file.is_open())
                {
                    continue;
                }

                std::vector<std::byte> byte;
                byte.resize(AAsset_getLength64(asset));
                AAsset_read(asset,byte.data(),byte.size());
                AAsset_close(asset);
                file.write(reinterpret_cast<char*>(byte.data()),byte.size());
            }
            AAssetDir_close(dir);
            dir = nullptr;
        }
    }





    //app_->activity->instance = this;

    android_app_set_motion_event_filter(app_, OnMotionEventFilter);
    android_app_set_key_event_filter(app_,OnKeyEventFilter);

    int events;
    android_poll_source *pSource;
    do {
        // Process all pending events before running game logic.
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0)
        {
            if (pSource)
            {
                pSource->process(app_, pSource);
            }

        }
    } while (command != APP_CMD_INIT_WINDOW);

    maxWindowSize_.x = ANativeWindow_getWidth(app_->window);
    maxWindowSize_.y = ANativeWindow_getHeight(app_->window);
    windowSize_ = maxWindowSize_;
    DebugIO.Log("Init");

    onPause = []()
    {
        DebugIO.Log("onPause");
        graphics->ResizeBackBuffer({0.0f,0.0f});
    };

    onResume = [this](ANativeWindow* window)
    {
        DebugIO.Log("Recreate");
        if (graphics != nullptr)
        {

            app_->window = window;
            isResized = true;
        }
    };

    app_->activity->callbacks->onPause = [](GameActivity* activity)
    {
        onPause();
    };

    app_->activity->callbacks->onStop = [](GameActivity* activity)
    {
        DebugIO.Log("onStop");
    };

    app_->activity->callbacks->onStart = [](GameActivity* activity)
    {
        DebugIO.Log("onStart");
    };


    app_->activity->callbacks->onWindowInsetsChanged = [](GameActivity* activity)
    {
        DebugIO.Log("onWindowInsetsChanged");
    };

    app_->activity->callbacks->onNativeWindowDestroyed = [](GameActivity* activity,ANativeWindow* wnd)
    {
        DebugIO.Log("onNativeWindowDestroyed={}",graphics != nullptr);
    };

    app_->activity->callbacks->onNativeWindowCreated =[](GameActivity* activity,ANativeWindow* wnd)
    {
        onResume(wnd);
    };

    reSize = [this](android_app* app,int x, int y)
    {
        graphics->ResizeBackBuffer({static_cast<float>(x),static_cast<float>(y)});
    };

    app_->activity->callbacks->onWindowFocusChanged= [](GameActivity* activity,bool flag)
    {
        DebugIO.Log("changed={}",flag);
    };

    app_->activity->callbacks->onTrimMemory = [](GameActivity* activity,int val)
    {
        DebugIO.Log("onTrimMemory={}",val);
    };

    app_->activity->callbacks->onSaveInstanceState = [](GameActivity* activity,SaveInstanceStateRecallback save, void* data)
    {
        DebugIO.Log("onSaveInstanceState");
    };
    DebugIO.Log("SystemInit終了");
}

Eugene::AndroidSystem::~AndroidSystem()
{
}

std::pair<Eugene::Graphics*, Eugene::GpuEngine*> Eugene::AndroidSystem::CreateGraphics(std::uint32_t bufferNum, std::uint64_t maxSize) const
{
    if (graphics && gpuEngine)
    {
        throw CreateErrorException("すでにGraphicsは生成されています");
    }

    graphics = new VkGraphics{app_,maxWindowSize_,gpuEngine,bufferNum,maxSize};
    return {graphics,gpuEngine};
}

bool Eugene::AndroidSystem::Update(void)
{
    int events;
    android_poll_source *pSource;
    if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0)
    {
        if (pSource)
        {
            pSource->process(app_, pSource);
        }
    }
    isEnd_ = app_->destroyRequested;

    if (isResized)
    {
        isResized = false;
        graphics->ResizeBackBuffer(maxWindowSize_,app_->window);
    }

    return !isEnd_;
}

bool Eugene::AndroidSystem::GetMouse(Mouse& outMouse) const&
{
	return false;
}

bool Eugene::AndroidSystem::SetMouse(Mouse& inMouse) const
{
	return false;
}

bool Eugene::AndroidSystem::IsHitKey(KeyID keyID) const
{
	return false;
}

bool Eugene::AndroidSystem::GetKeyData(KeyDataSpan keyData) const
{
	return false;
}

bool Eugene::AndroidSystem::SetKeyCodeTable(KeyCodeTable& keyCodeTable)
{
	return false;
}

bool Eugene::AndroidSystem::GetGamePad(GamePad& pad, std::uint32_t idx) const
{
	return false;
}

bool Eugene::AndroidSystem::GetTouch(TouchData& pressed, TouchData& move, TouchData& released) const
{
    auto& input = app_->inputBuffers[app_->currentInputBuffer];
    pressed.touchCount_ = 0;
    released.touchCount_ = 0;
    move.touchCount_ = 0;
    for (auto i = 0; i < input.motionEventsCount; i++)
    {
        switch (input.motionEvents[i].action)
        {
            case AMOTION_EVENT_ACTION_DOWN:
                for (auto j = 0; j < input.motionEvents[i].pointerCount && pressed.touchCount_ < 8; j++)
                {
                    pressed.touchList_[pressed.touchCount_].pos.x = input.motionEvents[i].pointers[j].rawX;
                    pressed.touchList_[pressed.touchCount_].pos.y = input.motionEvents[i].pointers[j].rawY;
                    pressed.touchList_[pressed.touchCount_].nowTime = input.motionEvents[i].eventTime;
                    pressed.touchList_[pressed.touchCount_].downTime = input.motionEvents[i].downTime;
                    pressed.touchCount_++;
                }
            break;
            case AMOTION_EVENT_ACTION_UP:
                for (auto j = 0; j < input.motionEvents[i].pointerCount && released.touchCount_ < 8; j++)
                {
                    released.touchList_[released.touchCount_].pos.x = input.motionEvents[i].pointers[j].rawX;
                    released.touchList_[released.touchCount_].pos.y = input.motionEvents[i].pointers[j].rawY;
                    released.touchList_[released.touchCount_].nowTime = input.motionEvents[i].eventTime;
                    released.touchList_[released.touchCount_].downTime = input.motionEvents[i].downTime;
                    released.touchCount_++;
                }
            break;
            case AMOTION_EVENT_ACTION_MOVE:
                for (auto j = 0; j < input.motionEvents[i].pointerCount && move.touchCount_ < 8; j++)
                {
                    move.touchList_[move.touchCount_].pos.x = input.motionEvents[i].pointers[j].rawX;
                    move.touchList_[move.touchCount_].pos.y = input.motionEvents[i].pointers[j].rawY;
                    move.touchList_[move.touchCount_].nowTime = input.motionEvents[i].eventTime;
                    move.touchList_[move.touchCount_].downTime = input.motionEvents[i].downTime;
                    move.touchCount_++;
                }
            break;
            default:
            break;
        }
    }
    android_app_clear_motion_events(&input);
    return pressed.touchCount_ > 0 || move.touchCount_ > 0 || released.touchCount_ > 0;
}

bool Eugene::AndroidSystem::IsEnd(void) const
{
	return isEnd_;
}

void Eugene::AndroidSystem::OnResizeWindow(const glm::vec2& size)
{
}

void Eugene::AndroidSystem::OnSetFullScreen(bool isFullScreen)
{
}

Eugene::DynamicLibrary* Eugene::AndroidSystem::CreateDynamicLibrary(const std::filesystem::path& path) const
{
	return nullptr;
}

#ifdef USE_IMGUI
void Eugene::AndroidSystem::ImguiNewFrame(void) const
{
	ImGui_ImplWin32_NewFrame();
}

#endif
