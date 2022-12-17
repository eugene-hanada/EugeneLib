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
	return new Dx12UploadableResource{ device_.Get(),size};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateUploadableTextureResource(Texture& texture) const
{
	return new Dx12UploadableResource{ device_.Get(),texture};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateDefaultResource(size_t size) const
{
	return new Dx12DefaultResource{ device_.Get(),size};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateSwapChainResource(std::uint32_t idx) const
{
	return new Dx12DefaultResource{ swapChain_.Get(), idx};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateTextureResource(const TextureInfo& formatData) const
{
	return new Dx12DefaultResource{ device_.Get(), formatData};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateRenderTargetResource(const Vector2& size, Format format, const std::span<float, 4>& clearColor) const
{
	return new Dx12DefaultResource{ device_.Get(), size,format,clearColor};
}

EugeneLib::GpuResource* EugeneLib::Dx12Graphics::CreateDepthResource(const Vector2& size, Format format, float clearValue) const
{
	return new Dx12DefaultResource{device_.Get(),size,format, clearValue};
}

EugeneLib::ShaderResourceViews* EugeneLib::Dx12Graphics::CreateShaderResourceViews(size_t size) const
{
	return new Dx12ShaderResourceViews{ device_.Get(), size};
}

EugeneLib::RenderTargetViews* EugeneLib::Dx12Graphics::CreateRenderTargetViews(size_t size, bool isShaderVisible) const
{
	return new Dx12RenderTargetViews{ device_.Get(), size,isShaderVisible};
}

EugeneLib::VertexView* EugeneLib::Dx12Graphics::CreateVertexView(size_t size, size_t vertexNum, GpuResource& resource) const
{
	return new Dx12VertexView{size, vertexNum,resource};
}

EugeneLib::GpuEngine* EugeneLib::Dx12Graphics::CreateGpuEngine(size_t maxSize) const
{
	return new Dx12GpuEngine{ device_.Get(), maxSize};
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

	// アダプターの列挙用
	std::list<IDXGIAdapter*> adapters;

	// 目的の名前のアダプターを入れる
	IDXGIAdapter* tmpAdapter = nullptr;

	// アダプターを格納してく
	for (int i = 0; dxgiFactory_->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	// 格納したアダプターの中から目的の名前のアダプターを探す
	std::wstring strDesc;
	tmpAdapter = *adapters.begin();
	for (auto& adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc{};
		adpt->GetDesc(&adesc);			// アダプターの説明オブジェクトを取得する
		strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos || strDesc.find(L"AMD") != std::string::npos)
		{
			// 一致したら抜ける
			tmpAdapter = adpt;
			break;
		}
	}

	// フューチャーレベルの配列
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
			// 見つかったらやめる
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

	// 設定用のDESC
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

	// 幅
	swapchainDesc.Width = static_cast<int>(size.x);

	// 高さ
	swapchainDesc.Height = static_cast<int>(size.y);

	// ピクセルフォーマット
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ステレオ表示フラグ
	swapchainDesc.Stereo = false;

	// マルチサンプルの指定
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;

	// DXGI_USAGE_BACK_BUFFERこんままで
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// バッファーの数ダブルバッファ-なので2
	swapchainDesc.BufferCount = 2;

	// バックバッファ-は伸び縮みできるように設定
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;

	// フリップ後にすぐに破棄するように設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// 特に指定しないのでDXGI_ALPHA_MODE_UNSPECIFIEDで
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	// フルスクリーン-ウィンドウの切り替え可能に設定
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	ID3D12CommandQueue* cmdQueue{ static_cast<ID3D12CommandQueue*>(gpuEngine->GetQueue()) };

	// スワップチェインの生成
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
	// バックバッファのインデックスを取得
	return swapChain_->GetCurrentBackBufferIndex();
}

void EugeneLib::Dx12Graphics::Present(void)
{
	swapChain_->Present(1, 0);
}
