#include "WindowsSystem.h"
#include <Windows.h>
#include <filesystem>
#include "../../../Include/Common/Debug.h"
#include "../../../Include/Common/EugeneLibException.h"

#include "../../Graphics/DirectX12/Dx12Graphics.h"

#include "../../../Include/Common/Debug.h"

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

EugeneLib::System::Mouse mouse;

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
	switch (msg)
	{
	case WM_DESTROY:
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
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



EugeneLib::WindowsSystem::WindowsSystem(const Vector2& size, const std::u8string& title) :
    System{size,title}
{
	DebugLog(u8"Comの初期化");
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		throw LibInitException();
	}

	DebugLog(u8"ウィンドウクラスの登録");
	std::filesystem::path tmpTitle{ title };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = tmpTitle.c_str();
	windowClass.hInstance = GetModuleHandle(nullptr);
	if (!RegisterClassEx(&windowClass))
	{
		throw LibInitException();
	}

	DebugLog(u8"ウィンドウサイズの設定");
	// ウィンドウのサイズ設定
	RECT wSize{ 0,0,static_cast<long>(windowSize_.x), static_cast<long>(windowSize_.y) };
	if (!AdjustWindowRect(&wSize, WS_OVERLAPPEDWINDOW, false))
	{
		throw LibInitException();
	}

	DebugLog(u8"ウィンドウの生成");
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
	DebugLog(u8"ウィンドウの表示");
	ShowWindow(hwnd, SW_SHOW);

	key_.fill(false);
}

EugeneLib::WindowsSystem::~WindowsSystem()
{
	CoUninitialize();
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

EugeneLib::Graphics* EugeneLib::WindowsSystem::CreateGraphics(GpuEngine*& gpuEngine, size_t bufferNum) const&
{
	return new Dx12Graphics{hwnd,GetWindowSize(),gpuEngine, bufferNum };
}

bool EugeneLib::WindowsSystem::Update(void)
{
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT)
	{
		return false;
	}
	key_.fill(false);
	std::uint8_t k[256];
	if (GetKeyboardState(k))
	{
		for (auto id : KeyID())
		{
			key_[std::underlying_type<KeyID>::type(id)] = k[codeTable_[std::underlying_type<KeyID>::type(id)]] & 0x80;
		}
	}
	return true;
}

void EugeneLib::WindowsSystem::GetMouse(Mouse& outMouse) const&
{
	outMouse = mouse;
}

bool EugeneLib::WindowsSystem::IsHitKey(KeyID keyID) const
{
	return key_[static_cast<int>(keyID)];
}

bool EugeneLib::WindowsSystem::GetKeyData(KeyData& keyData) const
{
	keyData = key_;
	return true;
}

bool EugeneLib::WindowsSystem::SetKeyCodeTable(KeyCodeTable& keyCodeTable)
{
	codeTable_ = keyCodeTable;
	return true;
}
