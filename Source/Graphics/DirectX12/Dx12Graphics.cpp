#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include "../../../Include/System/System.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3D12MemAlloc.h>
#include <list>
#include <string>
#include <cassert>
#include "../../../Include/Utils/EugeneLibException.h"
#include "../../../Include/Graphics/Shader.h"

#include "../../../Include/ThirdParty/glm/glm/vec3.hpp"

#ifdef USE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>

#endif

#include "../../../Include/Debug/Debug.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")




Eugene::Graphics::Graphics(GpuEngine& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum)
{
	CreateDevice();
	instance_.reset(this);

	auto hwnd = static_cast<HWND>(System::GetInstance().GetWindow());
	CreateSwapChain(hwnd, System::GetInstance().GetWindowSize(), gpuEngine, bufferNum, maxNum);
	CreateBackBuffers(bufferNum);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS f;
	f.Format = static_cast<DXGI_FORMAT>(FormatToDxgiFormat_.at(static_cast<std::size_t>(backBufferFormat_)));
	f.SampleCount = 1;

	while (SUCCEEDED(device_->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&f,
		sizeof(f))))
	{
		
		f.SampleCount++;
	}
	multiSampleCount_ = f.SampleCount - 1;

#ifdef USE_IMGUI
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256,D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
	if (FAILED(device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(imguiDescriptorHeap_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("DirectX12ディスクリプタヒープの作成に失敗");
	}

	ImGui_ImplDX12_Init(
		device_.Get(),
		bufferNum, 
		DXGI_FORMAT_R8G8B8A8_UNORM,
		imguiDescriptorHeap_.Get(),
		imguiDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		imguiDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
	);
	
#endif
}

Eugene::Graphics::~Graphics()
{
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
#endif

	swapChain_->SetFullscreenState(false, nullptr);
	if (swapChain_ != nullptr)
	{
		swapChain_->Release();
		swapChain_.Detach();
	}
}

void Eugene::Graphics::CreateDevice(void)
{
	UINT flagsDXGI = 0;

#ifdef _DEBUG
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	if (FAILED(CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("DXGIファクトリーの生成に失敗");
	}



	// アダプター格納用
	IDXGIAdapter* tmpAdapter = nullptr;

	// 最もパフォーマンスの高いGPUを選択
	dxgiFactory_->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter));

	
	// フューチャーレベル
	D3D_FEATURE_LEVEL levels[]{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	
	for (auto& level : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, level, IID_PPV_ARGS(device_.ReleaseAndGetAddressOf()))))
		{
			D3D12MA::ALLOCATOR_DESC allocatorDesc{};
			allocatorDesc.pAdapter = tmpAdapter;
			allocatorDesc.pDevice = device_.Get();
			if (FAILED(D3D12MA::CreateAllocator(&allocatorDesc, allocator_.ReleaseAndGetAddressOf())))
			{
				throw CreateErrorException("D3D12MAのアロケーター生成失敗");
			}
			tmpAdapter->Release();
			return;
		}
	}

	throw CreateErrorException("D3D12デバイスの作成に失敗");
}

void Eugene::Graphics::CreateSwapChain(HWND& hwnd, const glm::vec2& size, GpuEngine& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum)
{
	gpuEngine = GpuEngine{ maxNum };

	// スワップチェインDESC
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

	// 幅
	swapchainDesc.Width = static_cast<int>(size.x);

	// 高さ
	swapchainDesc.Height = static_cast<int>(size.y);

	// フォーマット�
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	backBufferFormat_ = Format::R8G8B8A8_UNORM;

	// ステレオ表示フラグｰ
	swapchainDesc.Stereo = false;

	// マルチサンプリングの設定
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;

	// DXGI_USAGE_BACK_BUFFERにしとく
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapchainDesc.BufferCount = bufferNum;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = 0;


	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScrDesc{};
	fullScrDesc.Windowed = true;
	fullScrDesc.RefreshRate.Denominator = 1;
	fullScrDesc.RefreshRate.Numerator = 75;
	fullScrDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	fullScrDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	// スワップチェイン生成
	IDXGISwapChain1* swapchain = nullptr;
	if (FAILED(dxgiFactory_->CreateSwapChainForHwnd(gpuEngine.cmdQueue_.Get(), hwnd, &swapchainDesc, &fullScrDesc, nullptr, &swapchain)))
	{
		throw CreateErrorException("スワップチェイン生成失敗");
	}
	if (FAILED(swapchain->QueryInterface(IID_PPV_ARGS(swapChain_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("スワップチェイン生成失敗");
	}

	dxgiFactory_->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	swapchain->Release();
}

void Eugene::Graphics::CreateBackBuffers(std::uint64_t bufferCount)
{
	buffers_.resize(bufferCount);
	renderTargetViews_.Init(bufferCount, false);
	for (size_t i = 0; i < bufferCount; i++)
	{
		buffers_[i] = ImageResource{ static_cast<std::uint32_t>(i)};

		renderTargetViews_.Create(buffers_[i], i);
	}

}

Eugene::ImageResource& Eugene::Graphics::GetBackBufferResource(std::uint64_t idx)
{
	assert(idx < buffers_.size());

	return buffers_[idx];
}


size_t Eugene::Graphics::GetNowBackBufferIndex(void) const
{
	// バックバッファのインデックスを取得
	return swapChain_->GetCurrentBackBufferIndex();
}

void Eugene::Graphics::Present(void)
{
	swapChain_->Present(1, 0);
}

void Eugene::Graphics::ResizeBackBuffer(const glm::vec2& size,void* window)
{
	for (auto& buffer : buffers_)
	{
		buffer.Final();
	}
	DXGI_SWAP_CHAIN_DESC1 dec;
	swapChain_->GetDesc1(&dec);
	swapChain_->ResizeBuffers(dec.BufferCount, static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y), dec.Format, dec.Flags);


	for (size_t i = 0; i < buffers_.size(); i++)
	{
		buffers_[i] =  ImageResource{static_cast<std::uint32_t>(i) };
		renderTargetViews_.Create(buffers_[i], i);
	}
}

void Eugene::Graphics::SetFullScreenFlag(bool isFullScreen)
{
	DXGI_SWAP_CHAIN_DESC1 dec;
	swapChain_->GetDesc1(&dec);
	swapChain_->SetFullscreenState(isFullScreen, nullptr);
}



std::pair<Eugene::GpuMemoryInfo, Eugene::GpuMemoryInfo> Eugene::Graphics::GetGpuMemoryInfo(void) const
{
	D3D12MA::Budget local, sys;
	allocator_->GetBudget(&local, &sys);
	return { {local.UsageBytes,local.BudgetBytes},{sys.UsageBytes,sys.BudgetBytes} };
}

#ifdef USE_IMGUI

void Eugene::Graphics::ImguiNewFrame(void) const
{
	ImGui_ImplDX12_NewFrame();
}

void* Eugene::Graphics::GetImguiImageID(std::uint64_t index) const
{
	if (index >= imguiImageMax_)
	{
		return nullptr;
	}
	auto ptr = imguiDescriptorHeap_.Get();
	auto handle = ptr->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (index + 1);
	return reinterpret_cast<void*>(handle.ptr);
}

void Eugene::Graphics::SetImguiImage(ImageResource& imageResource, std::uint64_t index)
{
	if (index >= imguiImageMax_)
	{
		return;
	}

	auto img{ static_cast<ID3D12Resource*>(imageResource.GetResource()) };
	auto imgDesc{ img->GetDesc() };
	auto handle = imguiDescriptorHeap_.Get()->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (index + 1);
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = imgDesc.Format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Texture2D.MipLevels = imgDesc.MipLevels;
	device_->CreateShaderResourceView(img, &desc, handle);
}
#endif

