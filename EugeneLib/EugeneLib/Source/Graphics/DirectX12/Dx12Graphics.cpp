#include "Dx12Graphics.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <list>
#include <string>
#include "../../../Include/Common/EugeneLibException.h"
#include "../../../Include/System/System.h"
#include "../../../Include/Graphics/GpuEngine.h"
#include "../../../Include/Graphics/GpuResource.h"
#include "../../../Include/Graphics/RenderTargetViews.h"
#include "../../../Include/Common/Debug.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

EugeneLib::Dx12Graphics::Dx12Graphics(System& system, GpuEngine*& outGpuEngine)
{
	CreateDevice();
	CreateSwapChain(system, outGpuEngine);
	CreateBackBuffers(2);
}

void* EugeneLib::Dx12Graphics::GetDevice(void) const
{
	return device_.Get();
}

void* EugeneLib::Dx12Graphics::GetSwapChain(void) const
{
	return swapChain_.Get();
}

void EugeneLib::Dx12Graphics::CreateDevice(void)
{
	UINT flagsDXGI = 0;

#ifdef _DEBUG
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	if (FAILED(CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf()))))
	{
		throw LibInitException();
	}

	// �A�_�v�^�[�̗񋓗p
	std::list<IDXGIAdapter*> adapters;

	// �ړI�̖��O�̃A�_�v�^�[������
	IDXGIAdapter* tmpAdapter = nullptr;

	// �A�_�v�^�[���i�[���Ă�
	for (int i = 0; dxgiFactory_->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	// �i�[�����A�_�v�^�[�̒�����ړI�̖��O�̃A�_�v�^�[��T��
	std::wstring strDesc;
	tmpAdapter = *adapters.begin();
	for (auto& adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc{};
		adpt->GetDesc(&adesc);			// �A�_�v�^�[�̐����I�u�W�F�N�g���擾����
		strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos || strDesc.find(L"AMD") != std::string::npos)
		{
			// ��v�����甲����
			tmpAdapter = adpt;
			break;
		}
	}

	// �t���[�`���[���x���̔z��
	D3D_FEATURE_LEVEL levels[]{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL fLavel;



	for (auto& level : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, level, IID_PPV_ARGS(device_.ReleaseAndGetAddressOf()))))
		{
			// �����������߂�
			fLavel = level;
			return;
		}
	}

	throw LibInitException();
}

void EugeneLib::Dx12Graphics::CreateSwapChain(System& system, GpuEngine*& gpuEngine)
{
	gpuEngine = CreateGpuEngine(10, *this);

	// �ݒ�p��DESC
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

	// ��
	swapchainDesc.Width = static_cast<int>(system.GetWindowSize().x);

	// ����
	swapchainDesc.Height = static_cast<int>(system.GetWindowSize().y);

	// �s�N�Z���t�H�[�}�b�g
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �X�e���I�\���t���O
	swapchainDesc.Stereo = false;

	// �}���`�T���v���̎w��
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;

	// DXGI_USAGE_BACK_BUFFER����܂܂ł悩
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �o�b�t�@�[�̐��_�u���o�b�t�@-�Ȃ̂�2
	swapchainDesc.BufferCount = 2;

	// �o�b�N�o�b�t�@-�͐L�яk�݂ł���悤�ɐݒ�
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;

	// �t���b�v��ɂ����ɔj������悤�ɐݒ�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// ���Ɏw�肵�Ȃ��̂�DXGI_ALPHA_MODE_UNSPECIFIED��
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	// �t���X�N���[��-�E�B���h�E�̐؂�ւ��\�ɐݒ�
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HWND hwnd{ *static_cast<HWND*>(system.GetWindowHandle()) };

	ID3D12CommandQueue* cmdQueue{ static_cast<ID3D12CommandQueue*>(gpuEngine->GetQueue()) };

	// �X���b�v�`�F�C���̐���
	IDXGISwapChain1* swapchain = nullptr;
	if (FAILED(dxgiFactory_->CreateSwapChainForHwnd(cmdQueue, hwnd, &swapchainDesc, nullptr, nullptr, &swapchain)))
	{
		throw LibInitException();
	}
	if (FAILED(swapchain->QueryInterface(IID_PPV_ARGS(swapChain_.ReleaseAndGetAddressOf()))))
	{
		throw LibInitException();
	}
}

void EugeneLib::Dx12Graphics::CreateBackBuffers(size_t bufferCount)
{
	buffers_.resize(bufferCount);
	renderTargetViews_.reset(CreateRenderTargetViews(*this, bufferCount, false));
	for (size_t i = 0; i < bufferCount; i++)
	{
		buffers_[i].reset(CreateSwapChainResource(static_cast<std::uint32_t>(i), *this));

		renderTargetViews_->Create(*buffers_[i], i, Format::R8G8B8A8_UNORM);
	}

}

EugeneLib::GpuResource& EugeneLib::Dx12Graphics::GetBackBufferResource(void)
{
	return *buffers_[GetNowBackBufferIndex()];
}

EugeneLib::RenderTargetViews& EugeneLib::Dx12Graphics::GetViews(void)
{
	return *renderTargetViews_;
}

size_t EugeneLib::Dx12Graphics::GetNowBackBufferIndex(void)
{
	// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾
	return swapChain_->GetCurrentBackBufferIndex();
}

void EugeneLib::Dx12Graphics::Present(void)
{
	swapChain_->Present(1, 0);
}
