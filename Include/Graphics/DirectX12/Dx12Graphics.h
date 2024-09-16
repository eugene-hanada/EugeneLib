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

	/// <summary>
	/// グラフィックスクラス
	/// </summary>
	class Graphics :
		public DynamicSingleton<Graphics>
	{
	public:
		~Graphics();

		/// <summary>
		/// Eugene::FormatからDXGIFORMATに変換するための定数配列
		/// </summary>
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

		/// <summary>
		/// 生成する
		/// </summary>
		/// <param name="bufferNum"> バックバッファの数 </param>
		/// <param name="initNum"> デフォルトのGPUエンジンのコマンドリスト数の初期値 </param>
		/// <returns> GPUエンジン </returns>
		static GpuEngine Create(std::uint32_t bufferNum = 2, std::uint64_t initNum = 100)
		{
			GpuEngine gpuEngine;
			new Graphics{ gpuEngine, bufferNum, initNum };
			return gpuEngine;
		}
	
		/// <summary>
		/// バックバッファのフォーマットを取得する
		/// </summary>
		/// <returns></returns>
		static const Format& BackBufferFormat()
		{
			return backBufferFormat_;
		}

		/// <summary>
		/// GpuEngineを生成する
		/// </summary>
		/// <param name="initSize"> GpuEngineのコマンドリストの初期数 </param>
		/// <returns> GpuEngine </returns>
		GpuEngine CreateGpuEngine(std::size_t initSize = 0) const
		{
			return { initSize };
		}

		/// <summary>
		/// CommandListを生成する
		/// </summary>
		/// <returns> CommandList </returns>
		CommandList CreateCommandList(void) const
		{
			return {nullptr};
		}

		/// <summary>
		/// アプロード用のBufferResourceを生成
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <returns> BufferResource </returns>
		BufferResource CreateUnloadableBufferResource(std::uint64_t size) const
		{
			return { size,false, GpuResourceType::Upload };
		}

		/// <summary>
		/// 読み戻しようのBufferResourceを生成
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <param name="isUnordered"> Unorderedで使用するか？ </param>
		/// <returns> BufferResource </returns>
		BufferResource CreateReadableBufferResource(std::uint64_t size, bool isUnordered = false) const
		{
			return { size, isUnordered, GpuResourceType::ReadBack };
		}

		/// <summary>
		/// BufferResourceを生成
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <param name="isUnordered"> Unorderedで使用するか？ </param>
		/// <returns> BufferResource </returns>
		BufferResource CreateBufferResource(std::uint64_t size, bool isUnordered = false) const
		{
			return { size, isUnordered, GpuResourceType::Default };
		}

		/// <summary>
		/// BufferResourceをImageをもとにアップロード用のBufferResourceを生成する
		/// </summary>
		/// <param name="image"> 画像 </param>
		/// <returns> BufferResource </returns>
		BufferResource CreateBufferResource(Image& image) const
		{
			return { image };
		}

		/// <summary>
		/// テクスチャ用画像情報をもとにImageResourceを生成する
		/// </summary>
		/// <param name="info">  </param>
		/// <returns> ImageResource </returns>
		ImageResource CreateImageResource(const TextureInfo& info) const
		{
			return { info };
		}

		/// <summary>
		/// ImageResourceをレンダーターゲットとして生成する
		/// </summary>
		/// <param name="size">　縦横のサイズ　</param>
		/// <param name="format"> フォーマット </param>
		/// <param name="arraySize"> 配列数 </param>
		/// <param name="mipLeveles"> ミップマップレベル </param>
		/// <param name="sampleCount"> サンプル数 </param>
		/// <param name="clearColor"> クリア時の色 </param>
		/// <returns> ImageResource </returns>
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

		/// <summary>
		/// ImageResourceをデプスステンシルとして生成する
		/// </summary>
		/// <param name="size"> 縦横のサイズ </param>
		/// <param name="clear"> クリア時の値 </param>
		/// <param name="sampleCount">　サンプル数　</param>
		/// <returns> ImageResource </returns>
		ImageResource CreateDepthResource(const glm::ivec2& size, float clear = 1, std::uint8_t sampleCount = 1) const
		{
			return { size, Format::R32_TYPELESS,clear,sampleCount };
		}

		/// <summary>
		/// RenderTargetViewsを生成する
		/// </summary>
		/// <param name="size"> ビューの数 </param>
		/// <param name="isShaderVisible"> シェーダー側から見えるか？ </param>
		/// <returns> RenderTargetViews </returns>
		RenderTargetViews CreateRenderTargetViews(std::uint32_t size, bool isShaderVisible) const
		{
			return { size, isShaderVisible };
		}

		/// <summary>
		/// DepthStencilViewsを生成する
		/// </summary>
		/// <param name="size"> ビューの数 </param>
		/// <param name="isShaderVisible"> シェーダ側から見えるか？ </param>
		/// <returns> DepthStencilViews </returns>
		DepthStencilViews CreateDepthStencilViews(std::uint32_t size, bool isShaderVisible) const
		{
			return { size, isShaderVisible };
		}

		/// <summary>
		/// バックバッファのリソースを取得する
		/// </summary>
		/// <param name="idx"> インデックス </param>
		/// <returns> バックバッファのImageResourceの参照 </returns>
		ImageResource& GetBackBufferResource(std::uint64_t idx);

		/// <summary>
		/// バックバッファのRenderTargetViewsを取得する
		/// </summary>
		/// <returns> RenderTargetViews </returns>
		RenderTargetViews& GetViews(void)
		{
			return renderTargetViews_;
		}

		/// <summary>
		/// 現在のバックバッファのインデックスを取得する
		/// </summary>
		/// <returns></returns>
		std::uint64_t GetNowBackBufferIndex(void) const;

		/// <summary>
		/// Present処理をする
		/// </summary>
		void Present(void);

		/// <summary>
		/// Samplerを生成する
		/// </summary>
		/// <param name="layout"> サンプラーのレイアウト </param>
		/// <returns> Sampler </returns>
		Sampler CreateSampler(const SamplerLayout& layout) const
		{
			return {layout};
		}

		/// <summary>
		/// バックバッファをリサイズする
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <param name="window"></param>
		void ResizeBackBuffer(const glm::vec2& size,void* window = nullptr);

		/// <summary>
		/// ResourceBindLayoutを生成する
		/// </summary>
		/// <param name="viewTypes"> ビューをバインドする際のレイアウト </param>
		/// <param name="flags"> ResourceBindFlag組み合わせ </param>
		/// <returns> ResourceBindLayout </returns>
		ResourceBindLayout CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlags flags) const
		{
			return { viewTypes,flags };
		}

		/// <summary>
		/// ResourceBindLayoutを生成する
		/// </summary>
		/// <param name="viewTypes"> ビューをバインドする際のレイアウト </param>
		/// <param name="flag"> リソースの使用方法のフラグ </param>
		/// <returns> ResourceBindLayout </returns>
		ResourceBindLayout CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlag flag) const
		{
			return { viewTypes,std::to_underlying(flag) };
		}

		/// <summary>
		/// グラフィックス用のPipelineを生成する
		/// </summary>
		/// <param name="resourceBindLayout"> リソースのバインド方法 </param>
		/// <param name="layout"> シェーダ入力のレイアウト </param>
		/// <param name="shaders"> シェーダ </param>
		/// <param name="rendertarges"> レンダーターゲット </param>
		/// <param name="topologyType"> トポロジーの種類 </param>
		/// <param name="isCulling"> カリングを行うのか？ </param>
		/// <param name="useDepth"> デプスバッファを使用するか？ </param>
		/// <param name="sampleCount"> サンプル数(msaa用) </param>
		/// <returns> Pipeline </returns>
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

		/// <summary>
		/// コンピュート用のPipelineを生成する
		/// </summary>
		/// <param name="resourceBindLayout"> リソースのバインド方法 </param>
		/// <param name="csShader"> コンピュートシェーダ </param>
		/// <returns> Pipeline </returns>
		Pipeline CreateComputePipeline(
			ResourceBindLayout& resourceBindLayout,
			const Shader& csShader
		) const
		{
			return { resourceBindLayout, csShader };
		}

		/// <summary>
		/// ShaderResourceViewsを生成する
		/// </summary>
		/// <param name="viewTypes"> リソースのバインドレイアウト </param>
		/// <returns> ShaderResourceViews </returns>
		ShaderResourceViews CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const
		{
			std::uint32_t num{ 0ul };
			for (std::uint64_t i = 0ull; i < viewTypes.size(); i++)
			{
				num += viewTypes.at(i).viewNum_;
			}
			return { num };
		}

		/// <summary>
		/// SamplerViewsを生成する
		/// </summary>
		/// <param name="viewTypes"> リソースのバインドレイアウト </param>
		/// <returns> SamplerViews </returns>
		SamplerViews CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const
		{
			std::uint32_t num{ 0ull };
			for (std::uint32_t i = 0ull; i < viewTypes.size(); i++)
			{
				num += viewTypes.at(i).viewNum_;
			}
			return {num };
		}

		/// <summary>
		/// GPUメモリの情報
		/// </summary>
		/// <returns></returns>
		std::pair<GpuMemoryInfo, GpuMemoryInfo> GetGpuMemoryInfo(void) const;

		/// <summary>
		/// 現在のバックバッファを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> ImageResourceの参照 </returns>
		ImageResource& GetBackBufferResource(void)
		{
			return GetBackBufferResource(GetNowBackBufferIndex());
		}

#ifdef EUGENE_IMGUI

		/// <summary>
		/// Imguiのフレーム開始処理
		/// </summary>
		void ImguiNewFrame(void) const;

		/// <summary>
		/// ImGui用の画像を取得する
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		void* GetImguiImageID(std::uint64_t index) const;

		/// <summary>
		/// ImGui用に画像をセットする
		/// </summary>
		/// <param name="imageResource"> 画像のリソース </param>
		/// <param name="index"> インデックス </param>
		void SetImguiImage(ImageResource& imageResource, std::uint64_t index = 0ull);
#endif
private:

		Graphics(GpuEngine& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);

		void CreateDevice(void);

		void CreateSwapChain(HWND& hwnd, const glm::vec2& size, GpuEngine& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);

		void CreateBackBuffers(std::uint64_t bufferCount);

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
#ifdef EUGENE_IMGUI

		/// <summary>
		/// imguiで使用する画像の最大数
		/// </summary>
		static constexpr std::uint64_t imguiImageMax_{ 1000ull };

		/// <summary>
		/// ImGui用のディスクリプタヒープ
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> imguiDescriptorHeap_;
#endif

	};
}
