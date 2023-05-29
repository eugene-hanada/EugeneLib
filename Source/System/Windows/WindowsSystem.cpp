#include "WindowsSystem.h"
#include <Windows.h>
#include <Xinput.h>
#include <filesystem>
#include <functional>
#include "Dll.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "../../../Include/Common/Debug.h"
#ifdef USE_VULKAN
#include "../../Graphics/Vulkan/VkGraphics.h"
#else
#include "../../Graphics/DirectX12/Dx12Graphics.h"
#endif

#ifdef USE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#endif

#include "../../../Include/Common/Debug.h"

#pragma comment(lib, "Xinput.lib")


/// <summary>
/// メッセージ
/// </summary>
MSG msg;

/// <summary>
/// ウィンドウクラス
/// </summary>
WNDCLASSEX windowClass;

/// <summary>
/// ウィンドウハンドル
/// </summary>
HWND hwnd;

Eugene::System::Mouse mouse;

Eugene::Graphics* graphics = nullptr;
Eugene::GpuEngine* gpuEngine = nullptr;


std::function<void(const Eugene::Vector2&)> resizeCall;

#ifdef USE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

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
	mouse.wheel = 0.0f;

#ifdef USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}
#endif

	switch (msg)
	{
	case WM_DESTROY:
		isEnd = true;
		PostQuitMessage(0);
		return 0;
	case WM_LBUTTONDOWN:
		mouse.left = true;
		return 0;
	case WM_LBUTTONUP:
		mouse.left = false;
		return 0;
	case WM_RBUTTONDOWN:
		mouse.right = true;
		return 0;
	case WM_RBUTTONUP:
		mouse.right = false;
		return 0;
	case WM_MOUSEMOVE:
		mouse.pos.x = LOWORD(lparam);
		mouse.pos.y = HIWORD(lparam);
		return 0;
	case WM_MOUSEWHEEL:
		mouse.wheel = GET_WHEEL_DELTA_WPARAM(wparam);
		return 0;
	case WM_SIZE:
		resizeCall({ static_cast<float>(LOWORD(lparam)), static_cast<float>(HIWORD(lparam)) });
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



Eugene::WindowsSystem::WindowsSystem(const Vector2& size, const std::u8string& title) :
    System{size,title}
{
	resizeCall = [this](const Vector2& size) {
		windowSize_ = size;
		if (graphics)
		{
			graphics->ResizeBackBuffer(size);
		}

	};

	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		throw EugeneLibException("Comの初期化に失敗");
	}

	std::filesystem::path tmpTitle{ title };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = tmpTitle.c_str();
	windowClass.hInstance = GetModuleHandle(nullptr);
	if (!RegisterClassEx(&windowClass))
	{
		throw EugeneLibException("ウィンドウクラスの登録に失敗");
	}

	// ウィンドウのサイズ設定
	RECT wSize{ 0,0,static_cast<long>(windowSize_.x), static_cast<long>(windowSize_.y) };
	if (!AdjustWindowRect(&wSize, WS_OVERLAPPEDWINDOW, false))
	{
		throw EugeneLibException("ウィンドウサイズ調整に失敗");
	}

	// ウィンドウの生成
	hwnd = CreateWindow(
		windowClass.lpszClassName,
		tmpTitle.c_str(),
		WS_OVERLAPPEDWINDOW,			// タイトルバーと境界線のあるウィンドウ
		CW_USEDEFAULT,					// OSに任せる
		CW_USEDEFAULT,
		wSize.right - wSize.left,		// ウィンドウ幅と高さ
		wSize.bottom - wSize.top,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);


	MONITORINFO monitorInfo;
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

	SetWindowPos(hwnd, nullptr, centerX, centerY, wSize.right - wSize.left, wSize.bottom - wSize.top, false);

	ShowWindow(hwnd, SW_SHOW);

#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplWin32_Init(hwnd);

	context_ = ImGui::GetCurrentContext();

#endif

}

Eugene::WindowsSystem::~WindowsSystem()
{

#ifdef USE_IMGUI
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
	CoUninitialize();
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}


Eugene::Graphics* Eugene::WindowsSystem::CreateGraphics(GpuEngine*& gpuEngine, std::uint32_t bufferNum) const&
{
	if (graphics != nullptr)
	{
		return graphics;
	}
#ifdef USE_VULKAN
	return (graphics = new VkGraphics{hwnd, GetWindowSize(),gpuEngine, bufferNum, 100ull });
#else
	return (graphics = new Dx12Graphics{hwnd,GetWindowSize(),gpuEngine, bufferNum, 100ull});
#endif
}

std::pair<Eugene::Graphics*, Eugene::GpuEngine*> Eugene::WindowsSystem::CreateGraphics(std::uint32_t bufferNum, std::uint64_t maxSize) const
{
	if (graphics == nullptr)
	{
#ifdef USE_VULKAN
		graphics = new VkGraphics{hwnd,GetWindowSize(),gpuEngine, bufferNum , maxSize };
#else
		graphics = new Dx12Graphics{ hwnd,GetWindowSize(),gpuEngine, bufferNum , maxSize };
#endif
	}
	return std::make_pair(graphics, gpuEngine);
}

bool Eugene::WindowsSystem::Update(void)
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

void Eugene::WindowsSystem::GetMouse(Mouse& outMouse) const&
{
	outMouse = mouse;
}

bool Eugene::WindowsSystem::IsHitKey(KeyID keyID) const
{
	return GetKeyState(codeTable_[std::underlying_type<KeyID>::type(keyID)]) & 0x8000;
}

bool Eugene::WindowsSystem::GetKeyData(KeyDataSpan keyData) const
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

bool Eugene::WindowsSystem::SetKeyCodeTable(KeyCodeTable& keyCodeTable)
{
	codeTable_ = keyCodeTable;
	return true;
}

bool Eugene::WindowsSystem::GetGamePad(GamePad& pad, std::uint32_t idx) const
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

bool Eugene::WindowsSystem::IsEnd(void) const
{
	return isEnd;
}

void Eugene::WindowsSystem::ResizeWindow(const Vector2& size)
{
	if (windowSize_ == size)
	{
		return;
	}

	windowSize_ = size;

	RECT wSize{ 0,0,static_cast<long>(windowSize_.x), static_cast<long>(windowSize_.y) };
	if (!AdjustWindowRect(&wSize, WS_OVERLAPPEDWINDOW, false))
	{
		throw EugeneLibException("ウィンドウサイズ調整に失敗");
	}

	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY), &monitorInfo);

	// モニターの座標とサイズを取得
	int monitorX = monitorInfo.rcWork.left;
	int monitorY = monitorInfo.rcWork.top;
	int monitorWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
	int monitorHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

	// 中心座標を計算
	int centerX = (monitorWidth -  static_cast<int>(wSize.right - wSize.left)) / 2;
	int centerY = (monitorHeight - static_cast<int>(wSize.bottom - wSize.top)) / 2;

	SetWindowPos(hwnd, nullptr, centerX, centerY, static_cast<int>(wSize.right - wSize.left), static_cast<int>(wSize.bottom - wSize.top), false);
	
	if (graphics)
	{
		graphics->ResizeBackBuffer(size);
	}
}

Eugene::DynamicLibrary* Eugene::WindowsSystem::CreateDynamicLibrary(const std::filesystem::path& path) const
{
	return new Dll{path};
}

#ifdef USE_IMGUI
void Eugene::WindowsSystem::ImguiNewFrame(void) const
{
	ImGui_ImplWin32_NewFrame();
}

#endif
