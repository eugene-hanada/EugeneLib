#include "../../../Include/System/System.h"
#include <Xinput.h>
#include <filesystem>
#include <functional>
#include "../../../Include/Utils/EugeneLibException.h"
#include "../../../Include/Debug/Debug.h"
#include "../../../Include/Graphics/Graphics.h"


#ifdef EUGENE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

#pragma comment(lib, "Xinput.lib")

namespace {

	/// <summary>
	/// メッセージ
	/// </summary>
	MSG msg;

	/// <summary>
	/// ウィンドウクラス
	/// </summary>
	WNDCLASSEX windowClass;
	
	/// <summary>
	/// ホイール
	/// </summary>
	float wheel = 0.0f;

	/// <summary>
	/// リサイズ時に処理を行う関数オブジェクト
	/// </summary>
	std::function<void(const glm::vec2&)> resizeCall;

	/// <summary>
	/// ウィンドウがアクティブもしくは非アクティブになった時の処理
	/// </summary>
	std::function<void(bool)> activateCall;

	/// <summary>
	/// 終了フラグ
	/// </summary>
	bool isEnd = false;

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wparam"></param>
	/// <param name="lparam"></param>
	/// <returns></returns>
	LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef EUGENE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		{
			return true;
		}
#endif

		switch (msg)
		{
		case WM_DESTROY:
			// ウィンドウ破棄時
			isEnd = true;
			PostQuitMessage(0);
			return 0;
		case WM_MOUSEWHEEL:
			// マウスホイールを動かしたとき
			wheel += static_cast<float>(GET_WHEEL_DELTA_WPARAM(wparam)) / static_cast<float>(WHEEL_DELTA);
			return 0;
		case WM_SIZE:
			// ウィンドウがリサイズされたとき
			resizeCall({ static_cast<float>(LOWORD(lparam)), static_cast<float>(HIWORD(lparam)) });
			return 0;
		case WM_ACTIVATE:
			// ウィンドウがアクティブもしくは非アクティブになった時
			if (activateCall)
			{
				activateCall(WA_INACTIVE != wparam);
			}
			return 0;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}
}

Eugene::System::System(const glm::vec2& size, const std::u8string& title, std::intptr_t other, std::span<std::string_view> directories):
	windowSize_{size},title_{title}, isActive_{true}
{
	resizeCall = [this](const glm::vec2& size) {
			windowSize_ = size;
			if (Graphics::GetInstance().IsCreate())
			{
				Graphics::GetInstance().ResizeBackBuffer(windowSize_);
			}
		};

	activateCall = [this](bool active) {
		isActive_ = active;
		};
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		throw EugeneLibException("Comの初期化に失敗");
	}

	auto mode = reinterpret_cast<const WindowMode&>(other);
	if (mode == WindowMode::None)
	{
		return;
	}

	std::filesystem::path tmpTitle{ title };
	::windowClass.cbSize = sizeof(WNDCLASSEX);
	::windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	::windowClass.lpszClassName = tmpTitle.c_str();
	::windowClass.hInstance = GetModuleHandle(nullptr);
	if (!RegisterClassEx(&::windowClass))
	{
		throw EugeneLibException("ウィンドウクラスの登録に失敗");
	}

	// ウィンドウのサイズ設定
	RECT wSize{ 0,0,static_cast<long>(windowSize_.x), static_cast<long>(windowSize_.y) };

	DWORD style = 0;

	switch (mode)
	{
	case WindowMode::Window:
		style = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		break;
	case WindowMode::Borderless:
		style = (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS) & (~WS_BORDER) & (~WS_DLGFRAME) & (~WS_THICKFRAME);
		style |= static_cast<long>(WS_POPUP);
		break;
	default:
		// 何もしない
		break;
	}
	

	if (!AdjustWindowRect(&wSize, style, false))
	{
		throw EugeneLibException("ウィンドウサイズ調整に失敗");
	}

	if (mode == WindowMode::Transparent)
	{
		hwnd = CreateWindow(
			::windowClass.lpszClassName,
			tmpTitle.c_str(),
			WS_POPUP,			// タイトルバーと境界線のあるウィンドウ
			CW_USEDEFAULT,					// OSに任せる
			CW_USEDEFAULT,
			wSize.right - wSize.left,		// ウィンドウ幅と高さ
			wSize.bottom - wSize.top,
			nullptr,
			nullptr,
			windowClass.hInstance,
			nullptr
		);
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST);
	}
	else
	{
		// ウィンドウの生成
		hwnd = CreateWindow(
			::windowClass.lpszClassName,
			tmpTitle.c_str(),
			style,			// タイトルバーと境界線のあるウィンドウ
			CW_USEDEFAULT,					// OSに任せる
			CW_USEDEFAULT,
			wSize.right - wSize.left,		// ウィンドウ幅と高さ
			wSize.bottom - wSize.top,
			nullptr,
			nullptr,
			windowClass.hInstance,
			nullptr
		);
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES);
	}
	MONITORINFO monitorInfo{};
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY), &monitorInfo);

	// モニターの座標とサイズを取得
	int monitorX = monitorInfo.rcWork.left;
	int monitorY = monitorInfo.rcWork.top;
	int monitorWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
	int monitorHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

	// 中心座標を計算
	int centerX =(monitorWidth - (wSize.right - wSize.left)) / 2;
	int centerY = (monitorHeight - (wSize.bottom - wSize.top))/ 2;

	maxWindowSize_ = {static_cast<float>(monitorWidth), static_cast<float>(monitorHeight)};

	SetWindowPos(hwnd, mode == WindowMode::Transparent ? HWND_TOPMOST : nullptr, centerX, centerY, wSize.right - wSize.left, wSize.bottom - wSize.top, false);

	ShowWindow(hwnd, SW_SHOW);

#ifdef EUGENE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplWin32_Init(hwnd);

	context_ = ImGui::GetCurrentContext();

#endif

}

Eugene::System::~System()
{

#ifdef EUGENE_IMGUI
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
	CoUninitialize();
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}



//std::pair<Eugene::Graphics*, Eugene::GpuEngine*> Eugene::System::CreateGraphics(std::uint32_t bufferNum, std::uint64_t maxSize) const
//{
//	if (graphics && gpuEngine)
//	{
//		throw EugeneLibException{"Graphics&GpuEngineはすでに生成されています"};
//	}
//
//	if (graphics == nullptr)
//	{
//#ifdef USE_VULKAN
//		graphics = new VkGraphics{hwnd, GetWindowSize(),gpuEngine, bufferNum , maxSize };
//#else
//		graphics = new Dx12Graphics{ hwnd,GetWindowSize(),gpuEngine, bufferNum , maxSize };
//#endif
//	}
//	return std::make_pair(graphics, gpuEngine);
//}

bool Eugene::System::Update(void)
{
	while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			isEnd = true;
		}
	}
	return !isEnd;
}

bool Eugene::System::GetMouse(Mouse& outMouse) const&
{
	outMouse.flags.set(static_cast<size_t>(Mouse::Flags::LeftButton), GetKeyState(VK_LBUTTON) & 0x8000);
	outMouse.flags.set(static_cast<size_t>(Mouse::Flags::RightButton), GetKeyState(VK_RBUTTON) & 0x8000);
	outMouse.flags.set(static_cast<size_t>(Mouse::Flags::CenterButton), GetKeyState(VK_MBUTTON) & 0x8000);
	outMouse.flags.set(static_cast<size_t>(Mouse::Flags::Other1Button), GetKeyState(VK_XBUTTON1) & 0x8000);
	outMouse.flags.set(static_cast<size_t>(Mouse::Flags::Other2Button), GetKeyState(VK_XBUTTON2) & 0x8000);
	POINT point{};
	if (!GetCursorPos(&point))
	{
		return false;
	}

	WINDOWINFO windowInfo{};
	windowInfo.cbSize = sizeof(WINDOWINFO);
	if (!GetWindowInfo(hwnd, &windowInfo))
	{
		return false;
	}

	outMouse.pos.x = static_cast<float>(point.x - windowInfo.rcClient.left);
	outMouse.pos.y = static_cast<float>(point.y - windowInfo.rcClient.top);
	outMouse.wheel = wheel;
	return true;
}

bool Eugene::System::SetMouse(Mouse& inMouse) const
{
	WINDOWINFO windowInfo{};
	windowInfo.cbSize = sizeof(WINDOWINFO);
	if (!GetWindowInfo(hwnd, &windowInfo))
	{
		// 取得失敗したのでfalseを返す
		return false;
	}

	if (!SetCursorPos(static_cast<int>(inMouse.pos.x) + windowInfo.rcClient.left, static_cast<int>(inMouse.pos.y) + windowInfo.rcClient.top))
	{
		// 失敗時falseを返す
		return false;
	}

	ShowCursor(inMouse.CheckFlags(Mouse::Flags::ShowCursor));
	return true;
}

bool Eugene::System::IsHitKey(KeyID keyID) const
{
	return GetKeyState(codeTable_[std::underlying_type<KeyID>::type(keyID)]) & 0x8000;
}

bool Eugene::System::GetKeyData(KeyDataSpan keyData) const
{
	std::uint8_t k[256];
	if (!GetKeyboardState(k))
	{
		return false;
	}
	for (auto id : KeyID())
	{
		keyData[std::underlying_type<KeyID>::type(id)] = k[codeTable_[std::underlying_type<KeyID>::type(id)]] & 0x80;
	}
	return true;
}

bool Eugene::System::SetKeyCodeTable(KeyCodeTable& keyCodeTable)
{
	codeTable_ = keyCodeTable;
	return true;
}

bool Eugene::System::GetGamePad(GamePad& pad, std::uint32_t idx) const
{
	XINPUT_STATE state;
	if (XInputGetState(idx, &state) != ERROR_SUCCESS)
	{
		return false;
	}
	
	for (int i = 0; i < static_cast<int>(pad.buttons.size()); i++)
	{
		if (state.Gamepad.wButtons != 0)
		{
			pad.buttons[i] = (state.Gamepad.wButtons & (0x0001 << i));
		}
	}

	pad.leftTrigger_ = static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f;
	pad.rightTrigger_ = static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f;
	pad.leftThumb_.x = static_cast<float>(state.Gamepad.sThumbLX) / 32767.0f;
	pad.leftThumb_.y = static_cast<float>(state.Gamepad.sThumbLY) / 32767.0f;
	pad.rightThumb_.x = static_cast<float>(state.Gamepad.sThumbRX) / 32767.0f;
	pad.rightThumb_.y = static_cast<float>(state.Gamepad.sThumbRY) / 32767.0f;

	return true;
}

bool Eugene::System::GetTouch(TouchData& pressed, TouchData& hold, TouchData& released) const
{
	return false;
}

bool Eugene::System::IsEnd(void) const
{
	return isEnd;
}

void Eugene::System::ResizeWindow(const glm::vec2& size)
{
	RECT wSize{ 0,0,static_cast<long>(windowSize_.x), static_cast<long>(windowSize_.y) };
	
	if (!AdjustWindowRect(&wSize, GetWindowLong(hwnd, GWL_STYLE), false))
	{
		throw EugeneLibException("ウィンドウサイズ調整に失敗");
	}

	MONITORINFO monitorInfo{};
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY), &monitorInfo);

	// モニターの座標とサイズを取得
	int monitorWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
	int monitorHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

	// 中心座標を計算
	int centerX = (monitorWidth -  static_cast<int>(wSize.right - wSize.left)) / 2;
	int centerY = (monitorHeight - static_cast<int>(wSize.bottom - wSize.top)) / 2;

	SetWindowPos(hwnd, nullptr, centerX, centerY, static_cast<int>(wSize.right - wSize.left), static_cast<int>(wSize.bottom - wSize.top), false);
}

void Eugene::System::SetFullScreen(bool isFullScreen)
{
	auto style = GetWindowLong(hwnd, GWL_STYLE);
	auto newStyle = 0;
	if (isFullScreen)
	{
		// フルスクリーン時、ボーダーレススタイルにする
		newStyle = style & (~WS_BORDER) & (~WS_DLGFRAME) & (~WS_THICKFRAME);
		newStyle |= static_cast<long>(WS_POPUP);
	}
	else
	{
		// 通常のウィンドウにする
		newStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	}
	SetWindowLongA(hwnd, GWL_STYLE, newStyle);
	
	// フルスクリーン時は最大化表示、そうでないときは通常通り表示する
	ShowWindow(hwnd, isFullScreen ? SW_SHOWMAXIMIZED : SW_SHOW);
}

#ifdef EUGENE_IMGUI
void Eugene::System::ImguiNewFrame(void) const
{
	ImGui_ImplWin32_NewFrame();
}

#endif
