#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include "../../../Include/System/System.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3D12MemAlloc.h>
#include <list>
#include <string>
#include <cassert>
#include "../../../Include/Utils/EugeneLibException.h"
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

#include "../../../Include/ThirdParty/glm/glm/vec3.hpp"

#ifdef USE_EFFEKSEER
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#endif

#ifdef USE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>

#endif

#include "../../../Include/Debug/Debug.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


const std::array<int,Eugene::FormatMax> Eugene::Graphics::FormatToDxgiFormat_
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

	// B8G8R8A8
	DXGI_FORMAT_B8G8R8A8_TYPELESS,
	DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
	DXGI_FORMAT_B8G8R8A8_TYPELESS,
	DXGI_FORMAT_B8G8R8A8_TYPELESS,
	DXGI_FORMAT_B8G8R8A8_TYPELESS,

	//
	DXGI_FORMAT_BC1_UNORM,
	DXGI_FORMAT_BC2_UNORM,
	DXGI_FORMAT_BC3_UNORM,
	DXGI_FORMAT_BC5_UNORM,
	DXGI_FORMAT_BC7_UNORM
};

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

Eugene::BufferResource* Eugene::Graphics::CreateUnloadableBufferResource(std::uint64_t size) const
{
	return new Dx12UnloadableBufferResource{allocator_.Get(), size};
}

Eugene::BufferResource* Eugene::Graphics::CreateReadableBufferResource(std::uint64_t size, bool isUnordered) const
{
	return new Dx12ReadableBuffeResource{allocator_.Get(),size, isUnordered};
}

Eugene::BufferResource* Eugene::Graphics::CreateBufferResource(std::uint64_t size, bool isUnordered) const
{
	return new Dx12BufferResource{ allocator_.Get(),size, isUnordered };
}

Eugene::BufferResource* Eugene::Graphics::CreateBufferResource(Image& texture) const
{
	return new Dx12UnloadableBufferResource{device_.Get(),allocator_.Get(), texture};
}

Eugene::ImageResource* Eugene::Graphics::CreateImageResource(const TextureInfo& formatData) const
{
	return new Dx12ImageResource{allocator_.Get(),formatData};
}

Eugene::ImageResource* Eugene::Graphics::CreateImageResource(
	const glm::ivec2& size, Format format,
	std::uint32_t arraySize,
	std::uint8_t mipLeveles,
	std::uint8_t sampleCount,
	std::optional<std::span<float, 4>> clearColor)
{
	if (format == Format::AUTO_BACKBUFFER)
	{
		format = backBufferFormat_;
	}
	return new Dx12ImageResource{ allocator_.Get(),size,format, arraySize, mipLeveles, sampleCount,clearColor };
}

Eugene::ImageResource* Eugene::Graphics::CreateDepthResource(const glm::ivec2& size, float clear, std::uint8_t sampleCount ) const
{
	return new Dx12ImageResource{allocator_.Get(), size, Format::R32_TYPELESS,clear,sampleCount};
}


Eugene::RenderTargetViews* Eugene::Graphics::CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const
{
	return new Dx12RenderTargetViews{ device_.Get(), size,isShaderVisible};
}

Eugene::DepthStencilViews* Eugene::Graphics::CreateDepthStencilViews(std::uint64_t size) const
{
	return new Dx12DepthStencilViews{ device_.Get(), size};
}

Eugene::VertexView* Eugene::Graphics::CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const
{
	return new Dx12VertexView{size, vertexNum,resource};
}

Eugene::IndexView* Eugene::Graphics::CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const
{
	if (format == Format::AUTO_BACKBUFFER)
	{
		format = backBufferFormat_;
	}

	return new Dx12IndexView{size,num, format,resource};
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
	renderTargetViews_.reset(CreateRenderTargetViews(bufferCount, false));
	for (size_t i = 0; i < bufferCount; i++)
	{
		buffers_[i].reset(new Dx12ImageResource{ swapChain_.Get(),static_cast<std::uint32_t>(i)});

		renderTargetViews_->Create(*buffers_[i], i);
	}

}

Eugene::ImageResource& Eugene::Graphics::GetBackBufferResource(std::uint64_t idx)
{
	assert(idx < buffers_.size());

	return *buffers_[idx];
}

Eugene::RenderTargetViews& Eugene::Graphics::GetViews(void)
{
	return *renderTargetViews_;
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

Eugene::Sampler* Eugene::Graphics::CreateSampler(const SamplerLayout& layout) const
{
	return new Dx12Sampler{ layout };
}



void Eugene::Graphics::ResizeBackBuffer(const glm::vec2& size,void* window)
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
		renderTargetViews_->Create(*buffers_[i], i);
	}
}

void Eugene::Graphics::SetFullScreenFlag(bool isFullScreen)
{
	DXGI_SWAP_CHAIN_DESC1 dec;
	swapChain_->GetDesc1(&dec);
	swapChain_->SetFullscreenState(isFullScreen, nullptr);
}

Eugene::ResourceBindLayout* Eugene::Graphics::CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlags flags) const
{
	return new Dx12ResourceBindLayout{device_.Get(), viewTypes,flags };
}

Eugene::Pipeline* Eugene::Graphics::CreateGraphicsPipeline(
	ResourceBindLayout& resourceBindLayout,
	const ArgsSpan<ShaderInputLayout>& layout, 
	const ArgsSpan<ShaderPair>& shaders,
	const ArgsSpan<RendertargetLayout>& rendertarges,
	TopologyType topologyType,
	bool isCulling,
	bool useDepth,
	std::uint8_t sampleCount 
) const
{
	return new Dx12Pipeline{device_.Get(),resourceBindLayout, layout, shaders, rendertarges, topologyType, isCulling, useDepth,sampleCount };
}

Eugene::Pipeline* Eugene::Graphics::CreateComputePipeline(ResourceBindLayout& resourceBindLayout, const Shader& csShader) const
{
	return new Dx12Pipeline{ device_.Get(), resourceBindLayout, csShader };
}

Eugene::ShaderResourceViews* Eugene::Graphics::CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const
{
	std::uint64_t num{ 0ull };
	for (std::uint64_t i = 0ull; i < viewTypes.size(); i++)
	{
		num += viewTypes.at(i).viewNum_;
	}
	return new Dx12ShaderResourceViews{device_.Get(),num };
}

Eugene::SamplerViews* Eugene::Graphics::CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const
{
	std::uint64_t num{ 0ull };
	for (std::uint64_t i = 0ull; i < viewTypes.size(); i++)
	{
		num += viewTypes.at(i).viewNum_;
	}
	return new Dx12SamplerViews{device_.Get(),num};
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

//#ifdef USE_EFFEKSEER
//
///// <summary>
///// Dx12用Effekseerラッパークラス
///// </summary>
//class Dx12EffekseerWarpper :
//	public Eugene::EffekseerWarpper
//{
//public:
//	Dx12EffekseerWarpper(ID3D12Device* device,ID3D12CommandQueue* cmdQueue, std::uint32_t swapchainCount,DXGI_FORMAT rtFormat, DXGI_FORMAT depthFormtat,bool reverseDepth, std::uint64_t maxNum) :
//		EffekseerWarpper{}
//	{
//		// レンダラー生成
//		DXGI_FORMAT rtF[]{ rtFormat };
//		renderer_ = EffekseerRendererDX12::Create(
//			device, cmdQueue, swapchainCount,
//			rtF, 1, depthFormtat, reverseDepth, maxNum
//		);
//		
//		// マネージャー生成
//		manager_ = Effekseer::Manager::Create(maxNum);
//
//		// レンダラーセット
//		manager_->SetSpriteRenderer(renderer_->CreateSpriteRenderer());
//		manager_->SetRibbonRenderer(renderer_->CreateRibbonRenderer());
//		manager_->SetRingRenderer(renderer_->CreateRingRenderer());
//		manager_->SetTrackRenderer(renderer_->CreateTrackRenderer());
//		manager_->SetModelRenderer(renderer_->CreateModelRenderer());
//
//		// ローダーセット
//		manager_->SetTextureLoader(renderer_->CreateTextureLoader());
//		manager_->SetModelLoader(renderer_->CreateModelLoader());
//		manager_->SetMaterialLoader(renderer_->CreateMaterialLoader());
//
//		// メモリープールとコマンドリストを生成
//		memoryPool_ = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer_->GetGraphicsDevice());
//		cmdList_ = EffekseerRenderer::CreateCommandList(renderer_->GetGraphicsDevice(), memoryPool_);
//		renderer_->SetCommandList(cmdList_);
//
//		auto viewerPosition = ::Effekseer::Vector3D(0.0f, 0.0f, -20.0f);
//		renderer_->SetCameraMatrix(
//			Effekseer::Matrix44().LookAtRH(
//				viewerPosition, Effekseer::Vector3D(0.0f, 0.0f, 0.0f), Effekseer::Vector3D(0.0f, 1.0f, 0.0f)
//			)
//		);
//	}
//
//	void Update(float delta) final
//	{
//		// 1フレームの経過時間を60フレーム基準での経過フレームに変換用
//		constexpr auto frameParSec = 1.0f / 60.0f;
//
//		// 開始処理
//		memoryPool_->NewFrame();
//
//		// 更新処理
//		manager_->Update(delta / frameParSec);
//	}
//
//	void Draw(Eugene::CommandList& cmdList) final
//	{
//		Effekseer::Manager::DrawParameter drawParameter;
//		drawParameter.ZNear = 0.0f;
//		drawParameter.ZFar = 1.0f;
//		drawParameter.ViewProjectionMatrix = renderer_->GetCameraProjectionMatrix();
//
//		EffekseerRendererDX12::BeginCommandList(cmdList_, static_cast<ID3D12GraphicsCommandList*>(cmdList.GetCommandList()));
//		renderer_->SetCommandList(cmdList_);
//		renderer_->BeginRendering();
//		manager_->Draw(drawParameter);
//		renderer_->EndRendering();
//		renderer_->SetCommandList(nullptr);
//		EffekseerRendererDX12::EndCommandList(cmdList_);
//	}
//	Effekseer::RefPtr<Effekseer::Manager>& GetManager()&
//	{
//		return manager_;
//	}
//
//	void SetCameraPos(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up) final
//	{
//		auto tmp = Effekseer::Matrix44().LookAtLH(
//			Effekseer::Vector3D{ eye.x,eye.y, eye.z }, Effekseer::Vector3D{ at.x, at.y, at.z }, Effekseer::Vector3D{ up.x, up.y, up.z });
//		renderer_->SetCameraMatrix(
//			tmp
//		);
//	}
//
//	void SetCameraProjection(float fov, float aspect, const glm::vec2 &nearfar) final
//	{
//		renderer_->SetProjectionMatrix(
//			Effekseer::Matrix44().PerspectiveFovLH(fov, aspect, nearfar.x, nearfar.y));
//	}
//
//	// EffekseerWarpper を介して継承されました
//	void SetCameraPos(const glm::mat4& mat) final
//	{
//		Effekseer::Matrix44 tmp;
//		for (int y = 0; y < 4; y++)
//		{
//			for (int x = 0; x < 4; x++)
//			{
//				tmp.Values[y][x] = mat[y][x];
//			}
//		}
//		renderer_->SetCameraMatrix(
//			tmp
//		);
//	}
//	void SetCameraProjection(const glm::mat4& mat) final
//	{
//		Effekseer::Matrix44 tmp;
//		for (int y = 0; y < 4; y++)
//		{
//			for (int x = 0; x < 4; x++)
//			{
//				tmp.Values[y][x] = mat[y][x];
//			}
//		}
//		renderer_->SetProjectionMatrix(tmp);
//	}
//private:
//
//	/// <summary>
//	/// レンダラー
//	/// </summary>
//	EffekseerRenderer::RendererRef renderer_;
//
//	/// <summary>
//	/// マネージャー
//	/// </summary>
//	Effekseer::RefPtr<Effekseer::Manager> manager_;
//
//	/// <summary>
//	/// メモリプール
//	/// </summary>
//	Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> memoryPool_;
//
//	/// <summary>
//	/// コマンドリスト
//	/// </summary>
//	Effekseer::RefPtr<EffekseerRenderer::CommandList> cmdList_;
//
//
//};
//
//Eugene::EffekseerWarpper* Eugene::Dx12Graphics::CreateEffekseerWarpper(
//	GpuEngine& gpuEngine, Format rtFormat, std::uint32_t rtNum, Format depthFormat,bool reverseDepth, std::uint64_t maxNumm
//) const
//{
//	if (rtFormat == Format::AUTO_BACKBUFFER)
//	{
//		rtFormat = backBufferFormat_;
//	}
//	auto rtF = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_[static_cast<int>(rtFormat)]);
//	auto depthF = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_[static_cast<int>(depthFormat)]);
//	DXGI_SWAP_CHAIN_DESC1 desc;
//	swapChain_->GetDesc1(&desc);
//	return new Dx12EffekseerWarpper{
//		device_.Get(), static_cast<ID3D12CommandQueue*>(gpuEngine.GetQueue()),
//		desc.BufferCount, rtF, depthF,reverseDepth,maxNumm
//	};
//}
//
//#endif
