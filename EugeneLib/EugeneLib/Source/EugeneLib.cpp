// EugeneLib.cpp : スタティック ライブラリ用の関数を定義します。
//
#include <Windows.h>
#include <filesystem>
#include "../pch.h"

WNDCLASSEX windowClass;

// ウィンドウプロシージャ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool EugeneLib::System::Init(const Vector2& wsize, const std::u8string& title)
{
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		return false;
	}

	std::filesystem::path tmpTitle{title};
	
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = tmpTitle.c_str();
	windowClass.hInstance = GetModuleHandle(nullptr);
	if (!RegisterClassEx(&windowClass))
	{
		return false;
	}
	// ウィンドウのサイズ設定
	RECT wSize{ 0,0,static_cast<long>(wsize.x), static_cast<long>(wsize.y) };
	if (!AdjustWindowRect(&wSize, WS_OVERLAPPEDWINDOW, false))
	{
		return false;
	}

	// ウィンドウの生成
	auto hwnd = CreateWindow(
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

	ShowWindow(hwnd, SW_SHOW);
	return false;
}

bool EugeneLib::System::End()
{
	return false;
}

bool EugeneLib::System::Update(void)
{
	return false;
}
