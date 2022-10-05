// EugeneLib.cpp : スタティック ライブラリ用の関数を定義します。
//
#include <Windows.h>
#include <filesystem>
#include "../Include/EugeneLib.h"
//#include "../Include/EugeneLibException.h"

WNDCLASSEX windowClass;
EugeneLib::Vector2 wsize;
MSG mes;

EugeneLib::System* EugeneLib::System::instance_;

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
	try
	{
		instance_ = new System(wsize, title);
	}
	catch (const std::exception& e)
	{
		delete instance_;
		throw e;
	}
	
	return true;
}

bool EugeneLib::System::End()
{
	delete instance_;
	return false;
}

bool EugeneLib::System::Update(void)
{
	if (PeekMessage(&mes, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&mes);
		DispatchMessage(&mes);
	}
	if (mes.message == WM_QUIT)
	{
		return false;
	}
	return true;
}

EugeneLib::System::System(const Vector2& wsize, const std::u8string& title)
{
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		//throw LibInitException();
	}

	std::filesystem::path tmpTitle{ title };

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = tmpTitle.c_str();
	windowClass.hInstance = GetModuleHandle(nullptr);
	if (!RegisterClassEx(&windowClass))
	{
		//throw LibInitException();
	}
	// ウィンドウのサイズ設定
	RECT wSize{ 0,0,static_cast<long>(wsize.x), static_cast<long>(wsize.y) };
	if (!AdjustWindowRect(&wSize, WS_OVERLAPPEDWINDOW, false))
	{
		//throw LibInitException();
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
}
