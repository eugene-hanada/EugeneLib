#include "WindowsSystem.h"
#include <Windows.h>
#include <filesystem>
#include "../../../Include/Common/Debug.h"
#include "../../../Include/Common/EugeneLibException.h"

#include "../../Graphics/DirectX12/Dx12Graphics.h"

/// <summary>
/// ���b�Z�[�W
/// </summary>
MSG msg;

/// <summary>
/// �E�B���h�E�N���X
/// </summary>
WNDCLASSEX windowClass;

/// <summary>
/// �E�B���h�E�n���h��
/// </summary>
HWND hwnd;

/// <summary>
/// �E�B���h�E�v���V�[�W��
/// </summary>
/// <param name="hwnd"></param>
/// <param name="msg"></param>
/// <param name="wparam"></param>
/// <param name="lparam"></param>
/// <returns></returns>
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
	DebugLog(u8"Com�̏�����");
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		throw LibInitException();
	}

	DebugLog(u8"�E�B���h�E�N���X�̓o�^");
	std::filesystem::path tmpTitle{ title };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = tmpTitle.c_str();
	windowClass.hInstance = GetModuleHandle(nullptr);
	if (!RegisterClassEx(&windowClass))
	{
		throw LibInitException();
	}

	DebugLog(u8"�E�B���h�E�T�C�Y�̐ݒ�");
	// �E�B���h�E�̃T�C�Y�ݒ�
	RECT wSize{ 0,0,static_cast<long>(windowSize_.x), static_cast<long>(windowSize_.y) };
	if (!AdjustWindowRect(&wSize, WS_OVERLAPPEDWINDOW, false))
	{
		throw LibInitException();
	}

	DebugLog(u8"�E�B���h�E�̐���");
	// �E�B���h�E�̐���
	hwnd = CreateWindow(
		windowClass.lpszClassName,
		tmpTitle.c_str(),
		WS_OVERLAPPEDWINDOW,			// �^�C�g���o�[�Ƌ��E���̂���E�B���h�E
		CW_USEDEFAULT,					// OS�ɔC����
		CW_USEDEFAULT,
		wSize.right - wSize.left,		// �E�B���h�E���ƍ���
		wSize.bottom - wSize.top,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);
	DebugLog(u8"�E�B���h�E�̕\��");
	ShowWindow(hwnd, SW_SHOW);

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
	return true;
}
