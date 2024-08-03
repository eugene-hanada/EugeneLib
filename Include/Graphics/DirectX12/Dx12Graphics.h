#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include <array>
#include "../Graphics.h"
#include "../GraphicsCommon.h"
#include "../../Utils/Utils.h"
#include "Dx12GpuEngine.h"
#include "Dx12CommandList.h"

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
	class GpuEngine;
	class CommandList;
	class BufferResource;
	class ImageResource;
	class RenderTargetViews;
	class DepthStencilViews;
	class VertexView;
	class IndexView;
	class Sampler;
	class Image;
	class ShaderResourceViews;
	class SamplerViews;

	class Graphics :
		public DynamicSingleton<Graphics>
	{
	public:
		~Graphics();

		static const std::array<int, FormatMax> FormatToDxgiFormat_;

		static GpuEngine Create(std::uint32_t bufferNum = 2, std::uint64_t maxNum = 100)
		{
			GpuEngine gpuEngine;
			new Graphics{ gpuEngine, bufferNum, maxNum };
			return gpuEngine;
		}
	
		static const Format& BackBufferFormat()
		{
			return backBufferFormat_;
		}

		GpuEngine CreateGpuEngine(std::size_t initSize) const
		{
			return { initSize };
		}

		CommandList CreateCommandList(void) const
		{
			return {};
		}

		BufferResource* CreateUnloadableBufferResource(std::uint64_t size) const;

		BufferResource* CreateReadableBufferResource(std::uint64_t size, bool isUnordered = false) const;

		BufferResource* CreateBufferResource(std::uint64_t size, bool isUnordered = false) const;

		BufferResource* CreateBufferResource(Image& texture) const;

		ImageResource* CreateImageResource(const TextureInfo& formatData) const;

		ImageResource* CreateImageResource(const glm::ivec2& size, Format format, 
			std::uint32_t arraySize = 1,
			std::uint8_t mipLeveles = 1,
			std::uint8_t sampleCount = 1,
			std::optional<std::span<float, 4>> clearColor = std::nullopt);

		ImageResource* CreateDepthResource(const glm::ivec2& size, float clear = 1, std::uint8_t sampleCount = 1) const;

		RenderTargetViews* CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const;

		DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const;

		VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const;

		IndexView* CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const;

		void CreateDevice(void);

		void CreateSwapChain(HWND& hwnd, const glm::vec2& size, GpuEngine& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);

		void CreateBackBuffers(std::uint64_t bufferCount);

		ImageResource& GetBackBufferResource(std::uint64_t idx);

		RenderTargetViews& GetViews(void);

		std::uint64_t GetNowBackBufferIndex(void) const;

		void Present(void);

		Sampler* CreateSampler(const SamplerLayout& layout) const;

		void ResizeBackBuffer(const glm::vec2& size,void* window = nullptr);

		void SetFullScreenFlag(bool isFullScreen);

		// Graphics を介して継承されました
		ResourceBindLayout* CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlags flags) const;

		// Graphics を介して継承されました
		Pipeline* CreateGraphicsPipeline(
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout, 
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges, 
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			bool useDepth = false,
			std::uint8_t sampleCount = 1
		) const;

		Pipeline* CreateComputePipeline(
			ResourceBindLayout& resourceBindLayout,
			const Shader& csShader
		) const;

		// Graphics を介して継承されました
		ShaderResourceViews* CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const;

		// Graphics を介して継承されました
		SamplerViews* CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const;

		std::pair<GpuMemoryInfo, GpuMemoryInfo> GetGpuMemoryInfo(void) const;

		ImageResource& GetBackBufferResource(void)
		{
			return GetBackBufferResource(GetNowBackBufferIndex());
		}

#ifdef USE_IMGUI
		void ImguiNewFrame(void) const;
		void* GetImguiImageID(std::uint64_t index) const;
		void SetImguiImage(ImageResource& imageResource, std::uint64_t index = 0ull);
#endif
private:

		Graphics(GpuEngine& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);

		// dxgi
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_{ nullptr };

		// DirectX12デバイス
		Microsoft::WRL::ComPtr<ID3D12Device> device_{ nullptr };

		Microsoft::WRL::ComPtr<D3D12MA::Allocator> allocator_;

		// スワップチェイン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_{ nullptr };
	
		std::vector<std::unique_ptr<ImageResource>> buffers_;

		std::unique_ptr<RenderTargetViews> renderTargetViews_;


		/// <summary>
		/// マルチサンプルアンチエイリアスで使える最大サンプル数
		/// </summary>
		std::uint8_t multiSampleCount_;

		/// <summary>
		/// バックバッファのフォーマット
		/// </summary>
		static inline Format backBufferFormat_;

		friend class System;
		friend class GpuEngine;
		friend class CommandList;
#ifdef USE_IMGUI

		/// <summary>
		/// imguiで使用する画像の最大数
		/// </summary>
		const std::uint64_t imguiImageMax_{ 1000ull };

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> imguiDescriptorHeap_;
#endif

#ifdef USE_EFFEKSEER
		class EffekseerWarpper* CreateEffekseerWarpper(
			GpuEngine& gpuEngine,
			Format rtFormat,
			std::uint32_t rtNum = 1,
			Format depthFormat = Format::NON,
			bool reverseDepth = false,
			std::uint64_t maxNumm = 8000
		) const;
#endif
		friend class Dx12CommandList;
};
}
