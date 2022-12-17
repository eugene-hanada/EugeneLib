#include "Dx12Graphics.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <list>
#include <string>
#include "../../../Include/Common/EugeneLibException.h"
#include "Dx12GpuEngine.h"
#include "Dx12CommandList.h"
#include "Dx12GraphicsPipeline.h"
#include "Dx12UploadableResource.h"
#include "Dx12DefaultResource.h"
#include "Dx12ShaderResourceViews.h"
#include "Dx12RenderTargetViews.h"
#include "Dx12VertexView.h"

#include "../../../Include/Common/Debug.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

EugeneLib::Dx12Graphics::Dx12Graphics(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine)
{
	CreateDevice();
	CreateSwapChain(hwnd, size,gpuEngine);
	CreateBackBuffers(2);
}

EugeneLib::Dx12Graphics::~Dx12Graphics()
{
}

EugeneLib::CommandList* EugeneLib::Dx12Graphics::CreateCommandList(void) const
{
	return new Dx12CommandList{device_.Get()};
}

EugeneLib::GraphicsPipeline* EugeneLib::Dx12Graphics::CreateGraphicsPipeline(ShaderInputSpan layout, ShaderTypePaisrSpan shaders, RenderTargetSpan rendertarges, TopologyType topologyType, bool isCulling, ShaderLayoutSpan shaderLayout, SamplerSpan samplerLayout) const
{
	return new Dx12GraphicsPipeline{
		device_.Get(),
		layout,
		shaders,
		rendertarges,
		topologyType,
		isCulling,
		shaderLayout,
		samplerLayout
	};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateUploadableResource(size_t size) const
{
	return new Dx12UploadableResource{size,device_.Get()};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateUploadableResource(Texture& texture) const
{
	return new Dx12UploadableResource{texture,device_.Get()};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateDefaultResource(size_t size) const
{
	return new Dx12DefaultResource{size, device_.Get()};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateSwapChainResource(std::uint32_t idx) const
{
	return new Dx12DefaultResource{ idx, swapChain_.Get()};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateTextureResource(const TextureInfo& formatData) const
{
	return new Dx12DefaultResource{formatData,device_.Get()};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateDefaultResource(const Vector2& size, Format format, const std::span<float, 4>& clearColor) const
{
	return new Dx12DefaultResource{size,format,clearColor,device_.Get()};
}

EugeneLib::ShaderResourceViews* EugeneLib::Dx12Graphics::CreateShaderResourceViews(size_t size) const
{
	return new Dx12ShaderResourceViews{size,device_.Get()};
}

EugeneLib::RenderTargetViews* EugeneLib::Dx12Graphics::CreateRenderTargetViews(size_t size, bool isShaderVisible) const
{
	return new Dx12RenderTargetViews{size,isShaderVisible,device_.Get()};
}

EugeneLib::VertexView* EugeneLib::Dx12Graphics::CreateVertexView(size_t size, size_t vertexNum, GpuResource& resource) const
{
	return new Dx12VertexView{size, vertexNum,resource};
}

EugeneLib::GpuEngine* EugeneLib::Dx12Graphics::CreateGpuEngine(size_t maxSize) const
{
	return new Dx12GpuEngine{ maxSize, device_.Get()};
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
			for (auto& a : adapters)
			{
				a->Release();
			}
			fLavel = level;
			return;
		}
	}

	throw LibInitException();
}

void EugeneLib::Dx12Graphics::CreateSwapChain(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine)
{
	gpuEngine = CreateGpuEngine(10);

	// �ݒ�p��DESC
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

	// ��
	swapchainDesc.Width = static_cast<int>(size.x);

	// ����
	swapchainDesc.Height = static_cast<int>(size.y);

	// �s�N�Z���t�H�[�}�b�g
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �X�e���I�\���t���O
	swapchainDesc.Stereo = false;

	// �}���`�T���v���̎w��
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;

	// DXGI_USAGE_BACK_BUFFER����܂܂�
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
	renderTargetViews_.reset(CreateRenderTargetViews(bufferCount, false));
	for (size_t i = 0; i < bufferCount; i++)
	{
		buffers_[i].reset(CreateSwapChainResource(static_cast<std::uint32_t>(i)));

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
