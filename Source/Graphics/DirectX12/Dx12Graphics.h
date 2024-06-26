﻿#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include <array>
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Graphics/GraphicsCommon.h"


struct ID3D12Device;
struct IDXGIFactory6;
struct IDXGISwapChain4;
struct ID3D12DescriptorHeap;

namespace D3D12MA
{
	struct Allocator;
}

namespace Eugene
{
	class Dx12Graphics :
		public Graphics
	{
	public:
		static const std::array<int, FormatMax> FormatToDxgiFormat_;
		Dx12Graphics(HWND& hwnd, const glm::vec2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);
		~Dx12Graphics();
	
		static const Format& BackBufferFormat()
		{
			return Graphics::backBufferFormat_;
		}
	private:
		GpuEngine* CreateGpuEngine(std::uint64_t maxSize) const final;

		CommandList* CreateCommandList(void) const final;

		BufferResource* CreateUploadableBufferResource(std::uint64_t size) const final;

		BufferResource* CreateBufferResource(std::uint64_t size) const final;

		BufferResource* CreateBufferResource(Image& texture) const final;

		ImageResource* CreateImageResource(const TextureInfo& formatData) const final;

		ImageResource* CreateImageResource(const glm::ivec2& size, Format format, 
			std::uint32_t arraySize = 1,
			std::uint8_t mipLeveles = 1,
			std::uint8_t sampleCount = 1,
			std::optional<std::span<float, 4>> clearColor = std::nullopt) final;

		ImageResource* CreateDepthResource(const glm::ivec2& size, float clear = 1, std::uint8_t sampleCount = 1) const final;

		RenderTargetViews* CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const final;

		DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const final;

		VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const final;

		IndexView* CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const final;

		void CreateDevice(void);

		void CreateSwapChain(HWND& hwnd, const glm::vec2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);

		void CreateBackBuffers(std::uint64_t bufferCount);

		ImageResource& GetBackBufferResource(std::uint64_t idx) final;

		RenderTargetViews& GetViews(void) final;

		std::uint64_t GetNowBackBufferIndex(void) const final;

		void Present(void) final;

		Sampler* CreateSampler(const SamplerLayout& layout) const final;

		void ResizeBackBuffer(const glm::vec2& size,void* window = nullptr) final;

		void SetFullScreenFlag(bool isFullScreen) final;

		// Graphics を介して継承されました
		ResourceBindLayout* CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes) const final;

		// Graphics を介して継承されました
		GraphicsPipeline* CreateGraphicsPipeline(
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout, 
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges, 
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			bool useDepth = false,
			std::uint8_t sampleCount = 1
		) const final;

		// Graphics を介して継承されました
		ShaderResourceViews* CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const final;

		// Graphics を介して継承されました
		SamplerViews* CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const final;

		std::pair<GpuMemoryInfo, GpuMemoryInfo> GetGpuMemoryInfo(void) const final;
#ifdef USE_IMGUI
		void ImguiNewFrame(void) const final;
		void* GetImguiImageID(std::uint64_t index) const final;
		void SetImguiImage(ImageResource& imageResource, std::uint64_t index = 0ull) final;
#endif

		// dxgi
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_{ nullptr };

		// DirectX12デバイス
		Microsoft::WRL::ComPtr<ID3D12Device> device_{ nullptr };

		Microsoft::WRL::ComPtr<D3D12MA::Allocator> allocator_;

		// スワップチェイン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_{ nullptr };
	
		std::vector<std::unique_ptr<ImageResource>> buffers_;
		std::unique_ptr<RenderTargetViews> renderTargetViews_;

#ifdef USE_IMGUI
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> imguiDescriptorHeap_;
#endif

#ifdef USE_EFFEKSEER
		EffekseerWarpper* CreateEffekseerWarpper(
			GpuEngine& gpuEngine,
			Format rtFormat,
			std::uint32_t rtNum = 1,
			Format depthFormat = Format::NON,
			bool reverseDepth = false,
			std::uint64_t maxNumm = 8000
		) const final;
#endif
		friend class Dx12CommandList;
};
}
