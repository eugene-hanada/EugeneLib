#include "WindowsSystem.h"
#include <Windows.h>
#include <Xinput.h>
#include <filesystem>
#include "../../../Include/Common/Debug.h"
#include "../../../Include/Common/EugeneLibException.h"

#include "../../Graphics/DirectX12/Dx12Graphics.h"

#include "../../../Include/Common/Debug.h"

#pragma comment(lib, "Xinput.lib")

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

Eugene::System::Mouse mouse;

Eugene::Graphics* graphics = nullptr;

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



Eugene::WindowsSystem::WindowsSystem(const Vector2& size, const std::u8string& title) :
    System{size,title}
{
	DebugLog("Com�̏�����");
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		throw LibInitException();
	}

	DebugLog("�E�B���h�E�N���X�̓o�^");
	std::filesystem::path tmpTitle{ title };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = tmpTitle.c_str();
	windowClass.hInstance = GetModuleHandle(nullptr);
	if (!RegisterClassEx(&windowClass))
	{
		throw LibInitException();
	}

	DebugLog("�E�B���h�E�T�C�Y�̐ݒ�");
	// �E�B���h�E�̃T�C�Y�ݒ�
	RECT wSize{ 0,0,static_cast<long>(windowSize_.x), static_cast<long>(windowSize_.y) };
	if (!AdjustWindowRect(&wSize, WS_OVERLAPPEDWINDOW, false))
	{
		throw LibInitException();
	}

	DebugLog("�E�B���h�E�̐���");
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
	DebugLog("�E�B���h�E�̕\��");
	ShowWindow(hwnd, SW_SHOW);
}

Eugene::WindowsSystem::~WindowsSystem()
{
	CoUninitialize();
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

Eugene::Graphics* Eugene::WindowsSystem::CreateGraphics(GpuEngine*& gpuEngine, size_t bufferNum) const&
{
	if (graphics != nullptr)
	{
		return graphics;
	}
	return (graphics = new Dx12Graphics{hwnd,GetWindowSize(),gpuEngine, bufferNum });
}

bool Eugene::WindowsSystem::Update(void)
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

void Eugene::WindowsSystem::GetMouse(Mouse& outMouse) const&
{
	outMouse = mouse;
}

bool Eugene::WindowsSystem::IsHitKey(KeyID keyID) const
{
	return GetKeyState(codeTable_[std::underlying_type<KeyID>::type(keyID)]) & 0x8000;
}

bool Eugene::WindowsSystem::GetKeyData(KeyDataSpan& keyData) const
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