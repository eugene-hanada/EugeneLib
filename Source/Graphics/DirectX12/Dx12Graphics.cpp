#include "Dx12Graphics.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <list>
#include <string>
#include <cassert>
#include "../../../Include/Common/EugeneLibException.h"
#include "Dx12GpuEngine.h"
#include "Dx12CommandList.h"
#include "Dx12GraphicsPipeline.h"
#include "Dx12ResourceBindLayout.h"

#include "Dx12BufferResource.h"
#include "Dx12ImageResource.h"

#include "Dx12ShaderResourceViews.h"
#include "Dx12RenderTargetViews.h"
#include "Dx12VertexView.h"
#include "Dx12IndexView.h"
#include "Dx12DepthStencilViews.h"
#include "Dx12Sampler.h"
#include "Dx12SamplerViews.h"
#include "../../../Include/Graphics/Shader.h"

#ifdef USE_EFFEKSEER
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#endif

#ifdef USE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>

#endif

#include "../../../Include/Common/Debug.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")



const std::array<int,Eugene::FormatMax> Eugene::Dx12Graphics::FormatToDxgiFormat_
{
	DXGI_FORMAT_UNKNOWN,

	// R32G32B32A32
	DXGI_FORMAT_R32G32B32A32_TYPELESS,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32A32_SINT,

	// R32G32B32
	DXGI_FORMAT_R32G32B32_TYPELESS,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,

	// R32G32
	DXGI_FORMAT_R32G32_TYPELESS,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,

	// R32
	DXGI_FORMAT_R32_TYPELESS,
	DXGI_FORMAT_D32_FLOAT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,

	// R16G16B16A16
	DXGI_FORMAT_R16G16B16A16_TYPELESS,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R16G16B16A16_SINT,

	// R16B16
	DXGI_FORMAT_R16G16_TYPELESS,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_R16G16_SINT,

	// R16
	DXGI_FORMAT_R16_TYPELESS,
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_D16_UNORM,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16_SINT,

	// R8G8B8A8
	DXGI_FORMAT_R8G8B8A8_TYPELESS,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R8G8B8A8_SINT,

	//
	DXGI_FORMAT_BC1_UNORM,
	DXGI_FORMAT_BC2_UNORM,
	DXGI_FORMAT_BC3_UNORM,
	DXGI_FORMAT_BC5_UNORM,
	DXGI_FORMAT_BC7_UNORM
};

Eugene::Dx12Graphics::Dx12Graphics(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum)
{
	CreateDevice();
	CreateSwapChain(hwnd, size,gpuEngine, bufferNum, maxNum);
	CreateBackBuffers(bufferNum);

#ifdef USE_IMGUI
	srViews_.reset(CreateShaderResourceViews(256));
	auto ptr = static_cast<ID3D12DescriptorHeap*>(srViews_->GetViews());
	ImGui_ImplDX12_Init(device_.Get(), bufferNum, DXGI_FORMAT_R8G8B8A8_UNORM, ptr, ptr->GetCPUDescriptorHandleForHeapStart(), ptr->GetGPUDescriptorHandleForHeapStart());
	
#endif
}

Eugene::Dx12Graphics::~Dx12Graphics()
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

Eugene::CommandList* Eugene::Dx12Graphics::CreateCommandList(void) const
{
	return new Dx12CommandList{device_.Get()};
}

Eugene::GraphicsPipeline* Eugene::Dx12Graphics::CreateGraphicsPipeline(
	ShaderInputSpan layout, ShaderTypePaisrSpan shaders,
	RenderTargetSpan rendertarges, TopologyType topologyType, 
	bool isCulling, bool useDepth, ShaderLayoutSpan shaderLayout, SamplerSpan samplerLayout
	) const
{
	return new Dx12GraphicsPipeline{
		device_.Get(),
		layout,
		shaders,
		rendertarges,
		topologyType,
		isCulling,
		shaderLayout,
		samplerLayout,
		useDepth
	};
}

Eugene::BufferResource* Eugene::Dx12Graphics::CreateUploadableBufferResource(std::uint64_t size) const
{
	return new Dx12UploadableBufferResource{device_.Get(), size};
}

Eugene::BufferResource* Eugene::Dx12Graphics::CreateBufferResource(std::uint64_t size) const
{
	return new Dx12BufferResource{ device_.Get(),size};
}

Eugene::BufferResource* Eugene::Dx12Graphics::CreateBufferResource(Image& texture) const
{
	return new Dx12UploadableBufferResource{device_.Get(), texture};
}

Eugene::ImageResource* Eugene::Dx12Graphics::CreateImageResource(const TextureInfo& formatData) const
{
	return new Dx12ImageResource{device_.Get(),formatData};
}

Eugene::ImageResource* Eugene::Dx12Graphics::CreateImageResource(const Vector2I& size, Format format, std::span<float, 4> clearColor)
{
	return new Dx12ImageResource{ device_.Get(),size,format, clearColor };
}

Eugene::ImageResource* Eugene::Dx12Graphics::CreateDepthResource(const Vector2I& size, float clear) const
{
	return new Dx12ImageResource{device_.Get(), size, Format::R32_TYPELESS,clear};
}

Eugene::ShaderResourceViews* Eugene::Dx12Graphics::CreateShaderResourceViews(std::uint64_t size) const
{
	return new Dx12ShaderResourceViews{ device_.Get(), size};
}

Eugene::RenderTargetViews* Eugene::Dx12Graphics::CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const
{
	return new Dx12RenderTargetViews{ device_.Get(), size,isShaderVisible};
}

Eugene::DepthStencilViews* Eugene::Dx12Graphics::CreateDepthStencilViews(std::uint64_t size) const
{
	return new Dx12DepthStencilViews{ device_.Get(), size};
}

Eugene::VertexView* Eugene::Dx12Graphics::CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const
{
	return new Dx12VertexView{size, vertexNum,resource};
}

Eugene::IndexView* Eugene::Dx12Graphics::CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const
{
	return new Dx12IndexView{size,num, format,resource};
}

Eugene::GpuEngine* Eugene::Dx12Graphics::CreateGpuEngine(std::uint64_t maxSize) const
{
	return new Dx12GpuEngine{ device_.Get(), maxSize};
}


void Eugene::Dx12Graphics::CreateDevice(void)
{
	UINT flagsDXGI = 0;

#ifdef _DEBUG
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	if (FAILED(CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("DXGIファクトリーの生成に失敗");
	}

	// アダプター列挙用リスト
	std::list<IDXGIAdapter*> adapters;

	// アダプター格納用
	IDXGIAdapter* tmpAdapter = nullptr;

	// アダプターを格納する
	for (int i = 0; dxgiFactory_->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	// 格納したアダプターから探す
	std::wstring strDesc;
	tmpAdapter = *adapters.begin();
	for (auto& adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc{};
		adpt->GetDesc(&adesc);			// アダプターについて取得する
		strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos || strDesc.find(L"AMD") != std::string::npos)
		{
			// 見つかったら抜ける
			tmpAdapter = adpt;
			
			break;
		}
	}

	// フューチャーレベル
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
			// 解放しとく
			for (auto& a : adapters)
			{
				a->Release();
			}
			fLavel = level;
			return;
		}
	}

	throw CreateErrorException("D3D12デバイスの作成に失敗");
}

void Eugene::Dx12Graphics::CreateSwapChain(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum)
{
	gpuEngine = CreateGpuEngine(maxNum);

	// スワップチェインDESC
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

	// 幅
	swapchainDesc.Width = static_cast<int>(size.x);

	// 高さ
	swapchainDesc.Height = static_cast<int>(size.y);

	// フォーマット�
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

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
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	ID3D12CommandQueue* cmdQueue{ static_cast<ID3D12CommandQueue*>(gpuEngine->GetQueue()) };

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScrDesc{};
	fullScrDesc.Windowed = true;
	fullScrDesc.RefreshRate.Denominator = 1;
	fullScrDesc.RefreshRate.Numerator = 75;
	fullScrDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	fullScrDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	// スワップチェイン生成
	IDXGISwapChain1* swapchain = nullptr;
	if (FAILED(dxgiFactory_->CreateSwapChainForHwnd(cmdQueue, hwnd, &swapchainDesc, &fullScrDesc, nullptr, &swapchain)))
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

void Eugene::Dx12Graphics::CreateBackBuffers(std::uint64_t bufferCount)
{
	buffers_.resize(bufferCount);
	renderTargetViews_.reset(CreateRenderTargetViews(bufferCount, false));
	for (size_t i = 0; i < bufferCount; i++)
	{
		buffers_[i].reset(new Dx12ImageResource{ swapChain_.Get(),static_cast<std::uint32_t>(i)});

		renderTargetViews_->Create(*buffers_[i], i, Format::R8G8B8A8_UNORM);
	}

}

Eugene::ImageResource& Eugene::Dx12Graphics::GetBackBufferResource(std::uint64_t idx)
{
	assert(idx < buffers_.size());

	return *buffers_[idx];
}

Eugene::RenderTargetViews& Eugene::Dx12Graphics::GetViews(void)
{
	return *renderTargetViews_;
}

size_t Eugene::Dx12Graphics::GetNowBackBufferIndex(void)
{
	// バックバッファのインデックスを取得
	return swapChain_->GetCurrentBackBufferIndex();
}

void Eugene::Dx12Graphics::Present(void)
{
	swapChain_->Present(1, 0);
}

Eugene::Sampler* Eugene::Dx12Graphics::CreateSampler(const SamplerLayout& layout) const
{
	return new Dx12Sampler{ layout };
}

Eugene::SamplerViews* Eugene::Dx12Graphics::CreateSamplerViews(size_t size) const
{
	return new Dx12SamplerViews{device_.Get(), size};
}

void Eugene::Dx12Graphics::ResizeBackBuffer(const Vector2& size)
{
	for (auto& buffer : buffers_)
	{
		buffer.reset();
	}
	DXGI_SWAP_CHAIN_DESC1 dec;
	swapChain_->GetDesc1(&dec);
	swapChain_->ResizeBuffers(dec.BufferCount, static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y), dec.Format, dec.Flags);


	for (size_t i = 0; i < buffers_.size(); i++)
	{
		buffers_[i].reset(new Dx12ImageResource{ swapChain_.Get(),static_cast<std::uint32_t>(i) });
		renderTargetViews_->Create(*buffers_[i], i, Format::R8G8B8A8_UNORM);
	}
}

void Eugene::Dx12Graphics::SetFullScreenFlag(bool isFullScreen)
{
	swapChain_->SetFullscreenState(isFullScreen, nullptr);
}

Eugene::ResourceBindLayout* Eugene::Dx12Graphics::CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes) const
{
	return new Dx12ResourceBindLayout{device_.Get(), viewTypes};
}

Eugene::GraphicsPipeline* Eugene::Dx12Graphics::CreateGraphicsPipeline(ResourceBindLayout& resourceBindLayout, ShaderInputSpan layout, ShaderTypePaisrSpan shaders, RenderTargetSpan rendertarges, TopologyType topologyType, bool isCulling, bool useDepth) const
{
	return new Dx12GraphicsPipeline{device_.Get(),resourceBindLayout, layout, shaders, rendertarges, topologyType, isCulling, useDepth};
}

#ifdef USE_IMGUI

void Eugene::Dx12Graphics::ImguiNewFrame(void) const
{
	ImGui_ImplDX12_NewFrame();
}
void* Eugene::Dx12Graphics::GetImguiImageID(std::uint64_t index) const
{
	auto ptr = static_cast<ID3D12DescriptorHeap*>(srViews_->GetViews());
	auto handle = ptr->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * index;
	return reinterpret_cast<void*>(handle.ptr);
}
Eugene::ShaderResourceViews& Eugene::Dx12Graphics::GetImguiShaderResourceView(void)&
{
	return *srViews_;
}

#endif

#ifdef USE_EFFEKSEER

/// <summary>
/// Dx12用Effekseerラッパークラス
/// </summary>
class Dx12EffekseerWarpper :
	public Eugene::EffekseerWarpper
{
public:
	Dx12EffekseerWarpper(ID3D12Device* device,ID3D12CommandQueue* cmdQueue, std::uint32_t swapchainCount,DXGI_FORMAT rtFormat, DXGI_FORMAT depthFormtat,bool reverseDepth, std::uint64_t maxNum) :
		EffekseerWarpper{}
	{
		// レンダラー生成
		DXGI_FORMAT rtF[]{ rtFormat };
		renderer_ = EffekseerRendererDX12::Create(
			device, cmdQueue, swapchainCount,
			rtF, 1, depthFormtat, reverseDepth, maxNum
		);
		
		// マネージャー生成
		manager_ = Effekseer::Manager::Create(maxNum);

		// レンダラーセット
		manager_->SetSpriteRenderer(renderer_->CreateSpriteRenderer());
		manager_->SetRibbonRenderer(renderer_->CreateRibbonRenderer());
		manager_->SetRingRenderer(renderer_->CreateRingRenderer());
		manager_->SetTrackRenderer(renderer_->CreateTrackRenderer());
		manager_->SetModelRenderer(renderer_->CreateModelRenderer());

		// ローダーセット
		manager_->SetTextureLoader(renderer_->CreateTextureLoader());
		manager_->SetModelLoader(renderer_->CreateModelLoader());
		manager_->SetMaterialLoader(renderer_->CreateMaterialLoader());

		// メモリープールとコマンドリストを生成
		memoryPool_ = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer_->GetGraphicsDevice());
		cmdList_ = EffekseerRenderer::CreateCommandList(renderer_->GetGraphicsDevice(), memoryPool_);
		renderer_->SetCommandList(cmdList_);

		auto viewerPosition = ::Effekseer::Vector3D(0.0f, 0.0f, -20.0f);
		renderer_->SetCameraMatrix(
			Effekseer::Matrix44().LookAtRH(
				viewerPosition, Effekseer::Vector3D(0.0f, 0.0f, 0.0f), Effekseer::Vector3D(0.0f, 1.0f, 0.0f)
			)
		);
	}

	void Update(float delta) final
	{
		// 1フレームの経過時間を60フレーム基準での経過フレームに変換用
		constexpr auto frameParSec = 1.0f / 60.0f;

		// 開始処理
		memoryPool_->NewFrame();

		// 更新処理
		manager_->Update(delta / frameParSec);
	}

	void Draw(Eugene::CommandList& cmdList) final
	{
		Effekseer::Manager::DrawParameter drawParameter;
		drawParameter.ZNear = 0.0f;
		drawParameter.ZFar = 1.0f;
		drawParameter.ViewProjectionMatrix = renderer_->GetCameraProjectionMatrix();

		EffekseerRendererDX12::BeginCommandList(cmdList_, static_cast<ID3D12GraphicsCommandList*>(cmdList.GetCommandList()));
		renderer_->SetCommandList(cmdList_);
		renderer_->BeginRendering();
		manager_->Draw(drawParameter);
		renderer_->EndRendering();
		renderer_->SetCommandList(nullptr);
		EffekseerRendererDX12::EndCommandList(cmdList_);
	}
	Effekseer::RefPtr<Effekseer::Manager>& GetManager()&
	{
		return manager_;
	}

	void SetCameraPos(const Eugene::Vector3& eye, const Eugene::Vector3& at, const Eugene::Vector3& up) final
	{
		renderer_->SetCameraMatrix(
			Effekseer::Matrix44().LookAtLH(
				Effekseer::Vector3D{ eye.x,eye.y, eye.z }, Effekseer::Vector3D{ at.x, at.y, at.z }, Effekseer::Vector3D{up.x, up.y, up.z}
			)
		);
	}

	void SetCameraProjection(float fov, float aspect, const Eugene::Vector2& nearfar) final
	{
		renderer_->SetProjectionMatrix(
			Effekseer::Matrix44().PerspectiveFovLH(fov, aspect, nearfar.x, nearfar.y));
	}
private:

	/// <summary>
	/// レンダラー
	/// </summary>
	EffekseerRenderer::RendererRef renderer_;

	/// <summary>
	/// マネージャー
	/// </summary>
	Effekseer::RefPtr<Effekseer::Manager> manager_;

	/// <summary>
	/// メモリプール
	/// </summary>
	Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> memoryPool_;

	/// <summary>
	/// コマンドリスト
	/// </summary>
	Effekseer::RefPtr<EffekseerRenderer::CommandList> cmdList_;
};

Eugene::EffekseerWarpper* Eugene::Dx12Graphics::CreateEffekseerWarpper(
	GpuEngine& gpuEngine, Format rtFormat, std::uint32_t rtNum, Format depthFormat,bool reverseDepth, std::uint64_t maxNumm
) const
{
	auto rtF = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_[static_cast<int>(rtFormat)]);
	auto depthF = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_[static_cast<int>(depthFormat)]);
	DXGI_SWAP_CHAIN_DESC1 desc;
	swapChain_->GetDesc1(&desc);
	return new Dx12EffekseerWarpper{
		device_.Get(), static_cast<ID3D12CommandQueue*>(gpuEngine.GetQueue()),
		desc.BufferCount, rtF, depthF,reverseDepth,maxNumm
	};
}

#endif
