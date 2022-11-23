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
			fLavel = level;
			return;
		}
	}

	throw LibInitException();
}

void EugeneLib::Dx12Graphics::CreateSwapChain(System& system, GpuEngine*& gpuEngine)
{
	gpuEngine = CreateGpuEngine(10, *this);

	// 設定用のDESC
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

	// 幅
	swapchainDesc.Width = static_cast<int>(system.GetWindowSize().x);

	// 高さ
	swapchainDesc.Height = static_cast<int>(system.GetWindowSize().y);

	// ピクセルフォーマット
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ステレオ表示フラグ
	swapchainDesc.Stereo = false;

	// マルチサンプルの指定
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;

	// DXGI_USAGE_BACK_BUFFERこんままでよか
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

	HWND hwnd{ *static_cast<HWND*>(system.GetWindowHandle()) };

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
	// バックバッファのインデックスを取得
	return swapChain_->GetCurrentBackBufferIndex();
}

void EugeneLib::Dx12Graphics::Present(void)
{
	swapChain_->Present(1, 0);
}
