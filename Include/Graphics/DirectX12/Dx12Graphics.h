#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include <array>
#include "../GraphicsCommon.h"
#include "../../Utils/Utils.h"
#include "Dx12GpuEngine.h"
#include "Dx12CommandList.h"
#include "Dx12ResourceBindLayout.h"
#include "Dx12GraphicsPipeline.h"
#include "Dx12ImageResource.h"
#include "Dx12BufferResource.h"
#include "Dx12RenderTargetViews.h"
#include "Dx12DepthStencilViews.h"
#include "Dx12ShaderResourceViews.h"
#include "Dx12Sampler.h"
#include "Dx12SamplerViews.h"
#include "Dx12VertexView.h"
#include "Dx12IndexView.h"

//struct ID3D12Device;
struct IDXGIFactory6;
struct IDXGISwapChain4;
//struct ID3D12DescriptorHeap;

namespace D3D12MA
{
	struct Allocator;
}

namespace Eugene
{
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
	struct SamplerLayout;

	class Graphics :
		public DynamicSingleton<Graphics>
	{
	public:
		~Graphics();

		static constexpr std::array<int, Eugene::FormatMax> FormatToDxgiFormat_
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

		BufferResource CreateUnloadableBufferResource(std::uint64_t size) const
		{
			return { size,false, GpuResourceType::Upload };
		}

		BufferResource CreateReadableBufferResource(std::uint64_t size, bool isUnordered = false) const
		{
			return { size, isUnordered, GpuResourceType::ReadBack };
		}

		BufferResource CreateBufferResource(std::uint64_t size, bool isUnordered = false) const
		{
			return { size, isUnordered, GpuResourceType::Default };
		}

		BufferResource CreateBufferResource(Image& image) const
		{
			return { image };
		}

		ImageResource CreateImageResource(const TextureInfo& info) const
		{
			return { info };
		}

		ImageResource CreateImageResource(const glm::ivec2& size, Format format,
			std::uint32_t arraySize = 1,
			std::uint8_t mipLeveles = 1,
			std::uint8_t sampleCount = 1,
			std::optional<std::span<float, 4>> clearColor = std::nullopt) const
		{
			if (format == Format::AUTO_BACKBUFFER)
			{
				format = backBufferFormat_;
			}
			return { size, format, arraySize, mipLeveles, sampleCount, clearColor };
		}

		ImageResource CreateDepthResource(const glm::ivec2& size, float clear = 1, std::uint8_t sampleCount = 1) const
		{
			return { size, Format::R32_TYPELESS,clear,sampleCount };
		}

		RenderTargetViews CreateRenderTargetViews(std::uint32_t size, bool isShaderVisible) const
		{
			return { size, isShaderVisible };
		}

		DepthStencilViews CreateDepthStencilViews(std::uint32_t size, bool isShaderVisible) const
		{
			return { size, isShaderVisible };
		}

		void CreateDevice(void);

		void CreateSwapChain(HWND& hwnd, const glm::vec2& size, GpuEngine& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);

		void CreateBackBuffers(std::uint64_t bufferCount);

		ImageResource& GetBackBufferResource(std::uint64_t idx);

		RenderTargetViews& GetViews(void)
		{
			return renderTargetViews_;
		}

		std::uint64_t GetNowBackBufferIndex(void) const;

		void Present(void);

		Sampler CreateSampler(const SamplerLayout& layout) const
		{
			return {layout};
		}

		void ResizeBackBuffer(const glm::vec2& size,void* window = nullptr);

		void SetFullScreenFlag(bool isFullScreen);

		// Graphics を介して継承されました
		ResourceBindLayout CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlags flags) const
		{
			return { viewTypes,flags };
		}

		ResourceBindLayout CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlag flag) const
		{
			return { viewTypes,std::to_underlying(flag) };
		}

		// Graphics を介して継承されました
		Pipeline CreateGraphicsPipeline(
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			bool useDepth = false,
			std::uint8_t sampleCount = 1
		) const
		{
			return { resourceBindLayout, layout, shaders, rendertarges, topologyType, isCulling, useDepth, sampleCount };
		}

		Pipeline CreateComputePipeline(
			ResourceBindLayout& resourceBindLayout,
			const Shader& csShader
		) const
		{
			return { resourceBindLayout, csShader };
		}

		// Graphics を介して継承されました
		ShaderResourceViews CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const
		{
			std::uint32_t num{ 0ul };
			for (std::uint64_t i = 0ull; i < viewTypes.size(); i++)
			{
				num += viewTypes.at(i).viewNum_;
			}
			return { num };
		}

		// Graphics を介して継承されました
		SamplerViews CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const
		{
			std::uint32_t num{ 0ull };
			for (std::uint32_t i = 0ull; i < viewTypes.size(); i++)
			{
				num += viewTypes.at(i).viewNum_;
			}
			return {num };
		}

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
	
		std::vector<ImageResource> buffers_;

		RenderTargetViews renderTargetViews_;


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
		friend class ImageResource;
		friend class BufferResource;
		friend class RenderTargetViews;
		friend class DepthStencilViews;
		friend class ShaderResourceViews;
		friend class SamplerViews;
		friend class ResourceBindLayout;
		friend class Pipeline;
#ifdef USE_IMGUI

		/// <summary>
		/// imguiで使用する画像の最大数
		/// </summary>
		const std::uint64_t imguiImageMax_{ 1000ull };

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> imguiDescriptorHeap_;
#endif

	};
}
