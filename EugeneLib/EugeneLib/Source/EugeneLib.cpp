#include "../Include/EugeneLib.h"

// EugeneLib.cpp : スタティック ライブラリ用の関数を定義します。
//#include "../framework.h"
//#include <wrl.h>
//#include <filesystem>
//#include <d3d12.h>
//#include <dxgi1_6.h>
//#include "../Include/EugeneLib.h"
//#include "../Include/Core/EugeneLibException.h"
//
//
//#pragma comment(lib,"d3d12.lib")
//#pragma comment(lib,"dxgi.lib")
//
//
//WNDCLASSEX windowClass;
//EugeneLib::Vector2 wsize;
//MSG mes;
//
//// Systemクラスのインスタンス
//EugeneLib::System* EugeneLib::System::instance_;
//
//// DirectX12のデバイス
//Microsoft::WRL::ComPtr<ID3D12Device> device{ nullptr };
//
//// ファクトリ
//Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory{ nullptr };
//
//
//// ウィンドウプロシージャ
//LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//	if (msg == WM_DESTROY)
//	{
//		PostQuitMessage(0);
//		return 0;
//	}
//	return DefWindowProc(hwnd, msg, wparam, lparam);
//}
//
//bool EugeneLib::System::Create(System*& outPtr,const Vector2& size, const std::u8string& title)
//{
//	try
//	{
//		instance_ = new System(size, title);
//	}
//	catch (const std::exception& e)
//	{
//		delete instance_;
//		throw e;
//	}
//	
//	outPtr = instance_;
//	return true;
//}
//
//bool EugeneLib::System::Update(void)
//{
//	if (PeekMessage(&mes, nullptr, 0, 0, PM_REMOVE))
//	{
//		TranslateMessage(&mes);
//		DispatchMessage(&mes);
//	}
//	if (mes.message == WM_QUIT)
//	{
//		return false;
//	}
//	return true;
//}
//
//EugeneLib::System::System(const Vector2& size, const std::u8string& title)
//{
//	wsize = size;
//	if (!CreateWnd(title))
//	{
//		throw LibInitException();
//	}
//	if (!CreateDevice())
//	{
//		throw LibInitException();
//	}
//}
//
//EugeneLib::System::~System()
//{
//	CoUninitialize();
//	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
//}
//
//bool EugeneLib::System::Relese(void)
//{
//	delete instance_;
//	instance_ = nullptr;
//	return true;
//}
//
//EugeneLib::System::ComPtr<ID3D12Device>& EugeneLib::System::GetDevice(void)
//{
//	return device;
//}
//
//
//bool EugeneLib::System::CreateWnd(const std::u8string& title)
//{
//	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
//	{
//		return false;
//	}
//
//	std::filesystem::path tmpTitle{ title };
//
//	windowClass.cbSize = sizeof(WNDCLASSEX);
//	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
//	windowClass.lpszClassName = tmpTitle.c_str();
//	windowClass.hInstance = GetModuleHandle(nullptr);
//	if (!RegisterClassEx(&windowClass))
//	{
//		return false;
//	}
//	// ウィンドウのサイズ設定
//	RECT wSize{ 0,0,static_cast<long>(wsize.x), static_cast<long>(wsize.y) };
//	if (!AdjustWindowRect(&wSize, WS_OVERLAPPEDWINDOW, false))
//	{
//		return false;
//	}
//
//	// ウィンドウの生成
//	auto hwnd = CreateWindow(
//		windowClass.lpszClassName,
//		tmpTitle.c_str(),
//		WS_OVERLAPPEDWINDOW,			// タイトルバーと境界線のあるウィンドウ
//		CW_USEDEFAULT,					// OSに任せる
//		CW_USEDEFAULT,
//		wSize.right - wSize.left,		// ウィンドウ幅と高さ
//		wSize.bottom - wSize.top,
//		nullptr,
//		nullptr,
//		windowClass.hInstance,
//		nullptr
//	);
//
//	ShowWindow(hwnd, SW_SHOW);
//
//	return true;
//}
//
//bool EugeneLib::System::CreateDevice(void)
//{
//	UINT flagsDXGI = 0;
//
//#ifdef _DEBUG
//	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
//#endif
//
//	if (FAILED(CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf()))))
//	{
//		return false;
//	}
//
//	// アダプターの列挙用
//	std::list<IDXGIAdapter*> adapters;
//
//	// 目的の名前のアダプターを入れる
//	IDXGIAdapter* tmpAdapter = nullptr;
//
//	// アダプターを格納してく
//	for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
//	{
//		adapters.push_back(tmpAdapter);
//	}
//
//	// 格納したアダプターの中から目的の名前のアダプターを探す
//	std::wstring strDesc;
//	tmpAdapter = *adapters.begin();
//	for (auto& adpt : adapters)
//	{
//		DXGI_ADAPTER_DESC adesc{};
//		adpt->GetDesc(&adesc);			// アダプターの説明オブジェクトを取得する
//		strDesc = adesc.Description;
//		if (strDesc.find(L"NVIDIA") != std::string::npos || strDesc.find(L"AMD") != std::string::npos)
//		{
//			// 一致したら抜ける
//			tmpAdapter = adpt;
//			break;
//		}
//	}
//
//	// フューチャーレベルの配列
//	D3D_FEATURE_LEVEL levels[]{
//		D3D_FEATURE_LEVEL_12_1,
//		D3D_FEATURE_LEVEL_12_0,
//		D3D_FEATURE_LEVEL_11_1,
//		D3D_FEATURE_LEVEL_11_0
//	};
//	D3D_FEATURE_LEVEL fLavel;
//
//
//
//	for (auto& level : levels)
//	{
//		if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, level, IID_PPV_ARGS(device.ReleaseAndGetAddressOf()))))
//		{
//			// 見つかったらやめる
//			fLavel = level;
//			device->SetName(TEXT("Device"));
//			return true;
//		}
//	}
//
//	return false;
//}
