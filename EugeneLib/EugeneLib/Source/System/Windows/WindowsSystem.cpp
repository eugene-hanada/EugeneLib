#include "WindowsSystem.h"
#include <Windows.h>
#include <filesystem>
#include "../../../Include/Common/Debug.h"
#include "../../../Include/Common/EugeneLibException.h"

MSG msg;
WNDCLASSEX windowClass;
HWND hwnd;

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
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

}

EugeneLib::WindowsSystem::~WindowsSystem()
{
	CoUninitialize();
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

void* EugeneLib::WindowsSystem::GetWindowHandle(void) const
{
	return &hwnd;
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
	return true;
}
