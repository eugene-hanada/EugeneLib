#pragma once
#include <vulkan/vulkan.hpp>
#include "../../Utils/Utils.h"
#include "../GraphicsCommon.h"
#include <vk_mem_alloc.hpp>
#include <memory>
#include <array>

#include "VkGpuEngine.h"
#include "VkGraphicsPipeline.h"
#include "VkResourceBindLayout.h"
#include "VkCommandList.h"
#include "VkBufferResource.h"
#include "VkImageResource.h"
#include "VkRenderTargetViews.h"
#include "VkDepthStencilViews.h"
#include "VkShaderResourceViews.h"
#include "VkSamplerViews.h"
#include "VkSampler.h"
#include "VkIndexView.h"
#include "VkVertexView.h"

#ifdef EUGENE_WINDOWS
#include <Windows.h>
#elif EUGENE_ANDROID
struct android_app;
#endif



#ifdef EUGENE_IMGUI
struct ImGui_ImplVulkanH_Window;
#endif

namespace Eugene
{
	/// <summary>
	/// グラフィックスクラス
	/// </summary>
	class Graphics :
		public DynamicSingleton<Graphics>
	{
	public:

		/// <summary>
		/// 生成する
		/// </summary>
		/// <param name="bufferNum"> バックバッファの数 </param>
		/// <param name="initNum"> デフォルトのGPUエンジンのコマンドリスト数の初期値 </param>
		/// <returns> GPUエンジン </returns>
		static GpuEngine Create(std::uint32_t bufferNum, std::uint64_t maxNum = 100)
		{
			GpuEngine gpuEngine;
			new Graphics{ gpuEngine, bufferNum, maxNum };
			return gpuEngine;
		}

		static void Create()
		{
			new Graphics{};
		}

		/// <summary>
		/// フォーマットをvk::Formatに変換するための定数配列
		/// </summary>
		static constexpr std::array<vk::Format, FormatMax> FormatToVkFormat{
			vk::Format::eUndefined,

			// R32G32B32A32
			vk::Format::eUndefined,
			vk::Format::eR32G32B32A32Sfloat,
			vk::Format::eR32G32B32A32Uint,
			vk::Format::eR32G32B32A32Sint,
		
			// R32G32B32
			vk::Format::eUndefined,
			vk::Format::eR32G32B32Sfloat,
			vk::Format::eR32G32B32Uint,
			vk::Format::eR32G32B32Sint,

			// R32G32
			vk::Format::eUndefined,
			vk::Format::eR32G32Sfloat,
			vk::Format::eR32G32Uint,
			vk::Format::eR32G32Sint,

			// R32
			vk::Format::eUndefined,
			vk::Format::eD32Sfloat,
			vk::Format::eR32Sfloat,
			vk::Format::eR32Uint,
			vk::Format::eR32Sint,

			// R16G16B16A16
			vk::Format::eUndefined,
			vk::Format::eR16G16B16A16Sfloat,
			vk::Format::eR16G16B16A16Unorm,
			vk::Format::eR16G16B16A16Uint,
			vk::Format::eR16G16B16A16Snorm,
			vk::Format::eR16G16B16A16Sint,

			// R16B16
			vk::Format::eUndefined,
			vk::Format::eR16G16Sfloat,
			vk::Format::eR16G16Unorm,
			vk::Format::eR16G16Uint,
			vk::Format::eR16G16Snorm,
			vk::Format::eR16G16Sint,

			// R16
			vk::Format::eUndefined,
			vk::Format::eR16Sfloat,
			vk::Format::eR16Unorm,
			vk::Format::eD16Unorm,
			vk::Format::eR16Uint,
			vk::Format::eR16Snorm,
			vk::Format::eR16Sint,

			// R8G8B8A8
			vk::Format::eUndefined,
			vk::Format::eR8G8B8A8Unorm,
			vk::Format::eR8G8B8A8Srgb,
			vk::Format::eR8G8B8A8Uint,
			vk::Format::eR8G8B8A8Snorm,
			vk::Format::eR8G8B8A8Sint,

			//
			vk::Format::eUndefined,
			vk::Format::eB8G8R8A8Unorm,
			vk::Format::eB8G8R8A8Srgb,
			vk::Format::eB8G8R8A8Uint,
			vk::Format::eB8G8R8A8Snorm,
			vk::Format::eB8G8R8A8Sint,

			// DDS
			vk::Format::eBc1RgbaUnormBlock,
			vk::Format::eBc2UnormBlock,
			vk::Format::eBc3UnormBlock,
			vk::Format::eBc5UnormBlock,
			vk::Format::eBc7UnormBlock,
		};

		/// <summary>
		/// バックバッファで使用しているフォーマットを取得する
		/// </summary>
		/// <returns> バックバッファで使用しているフォーマット </returns>
		static const Format& BackBufferFormat()
		{
			return Graphics::backBufferFormat_;
		}

		~Graphics();

		/// <summary>
		/// GpuEngineを生成する
		/// </summary>
		/// <param name="initSize"> GpuEngineのコマンドリストの初期数 </param>
		/// <returns> GpuEngine </returns>
		GpuEngine CreateGpuEngine(std::size_t initSize = 100) const
		{
			return { initSize };
		}

		/// <summary>
		/// CommandListを生成する
		/// </summary>
		/// <returns> CommandList </returns>
		CommandList CreateCommandList(void) const
		{
			return {};
		}
		
		/// <summary>
		/// BufferResourceを生成
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <param name="isUnordered"> Unorderedで使用するか？ </param>
		/// <returns> BufferResource </returns>
		BufferResource CreateReadableBufferResource(std::uint64_t size, bool isUnordered = false) const
		{
			return { size,isUnordered,GpuResourceType::ReadBack };
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
		BufferResource CreateBufferResource(Image& texture) const
		{
			return { texture };
		}

		/// <summary>
		/// アプロード用のBufferResourceを生成
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <returns> BufferResource </returns>
		BufferResource CreateUnloadableBufferResource(std::uint64_t size) const
		{
			return { size, false,GpuResourceType::Upload };
		}

		/// <summary>
		/// テクスチャ用画像情報をもとにImageResourceを生成する
		/// </summary>
		/// <param name="info">  </param>
		/// <returns> ImageResource </returns>
		ImageResource CreateImageResource(const TextureInfo& formatData) const
		{
			return { formatData };
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
		ImageResource CreateImageResource(
			const glm::ivec2& size,
			Format format,
			std::uint32_t arraySize = 1,
			std::uint8_t mipLeveles = 1,
			std::uint8_t sampleCount = 1,
			std::optional<std::span<float, 4>> clearColor = std::nullopt
		)
		{
			if (format == Format::AUTO_BACKBUFFER)
			{
				format = backBufferFormat_;
			}
			return { size, format,arraySize,mipLeveles,sampleCount,clearColor };
		}

		/// <summary>
		/// ShaderResourceViewsを生成する
		/// </summary>
		/// <param name="viewTypes"> リソースのバインドレイアウト </param>
		/// <returns> ShaderResourceViews </returns>
		ShaderResourceViews CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const
		{
			return { viewTypes };
		}

		/// <summary>
		/// DepthStencilViewsを生成する
		/// </summary>
		/// <param name="size"> ビューの数 </param>
		/// <param name="isShaderVisible"> シェーダ側から見えるか？ </param>
		/// <returns> DepthStencilViews </returns>
		DepthStencilViews CreateDepthStencilViews(std::uint32_t size) const
		{
			return { size };
		}

		/// <summary>
		/// RenderTargetViewsを生成する
		/// </summary>
		/// <param name="size"> ビューの数 </param>
		/// <param name="isShaderVisible"> シェーダー側から見えるか？ </param>
		/// <returns> RenderTargetViews </returns>
		RenderTargetViews CreateRenderTargetViews(std::uint32_t size, bool isShaderVisible) const
		{
			return { size };
		}

		/// <summary>
		/// バックバッファのリソースを取得する
		/// </summary>
		/// <param name="idx"> インデックス </param>
		/// <returns> バックバッファのImageResourceの参照 </returns>
		ImageResource& GetBackBufferResource(std::uint64_t idx)
		{
			return buffers_[idx];
		}

		/// <summary>
		/// バックバッファのRenderTargetViewsを取得する
		/// </summary>
		/// <returns> RenderTargetViews </returns>
		RenderTargetViews& GetViews(void) noexcept
		{
			return renderTargetViews_;
		}

		/// <summary>
		/// 現在のバックバッファのインデックスを取得する
		/// </summary>
		/// <returns></returns>
		std::uint64_t GetNowBackBufferIndex(void) const noexcept
		{
			return backBufferIdx_;
		}

		/// <summary>
		/// 現在のバックバッファを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> ImageResourceの参照 </returns>
		ImageResource& GetBackBufferResource() noexcept
		{
			return GetBackBufferResource(GetNowBackBufferIndex());
		}

		void Present(void);

		/// <summary>
		/// Samplerを生成する
		/// </summary>
		/// <param name="layout"> サンプラーのレイアウト </param>
		/// <returns> Sampler </returns>
		Sampler CreateSampler(const SamplerLayout& layout) const
		{
			return { layout };
		}

		/// <summary>
		/// SamplerViewsを生成する
		/// </summary>
		/// <param name="viewTypes"> リソースのバインドレイアウト </param>
		/// <returns> SamplerViews </returns>
		SamplerViews CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const
		{
			return { viewTypes };
		}

		/// <summary>
		/// バックバッファをリサイズする
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <param name="window"></param>
		void ResizeBackBuffer(const glm::vec2& size, void* window = nullptr);

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
			return {resourceBindLayout, layout, shaders, rendertarges, topologyType, isCulling, useDepth, sampleCount};
		}

		/// <summary>
		/// コンピュート用のPipelineを生成する
		/// </summary>
		/// <param name="resourceBindLayout"> リソースのバインド方法 </param>
		/// <param name="csShader"> コンピュートシェーダ </param>
		/// <returns> Pipeline </returns>
		Pipeline CreateComputePipeline(ResourceBindLayout& resourceBindLayout, const Shader& csShader) const
		{
			return { resourceBindLayout, csShader };
		}

		/// <summary>
		/// ResourceBindLayoutを生成する
		/// </summary>
		/// <param name="viewTypes"> ビューをバインドする際のレイアウト </param>
		/// <param name="flag"> リソースの使用方法のフラグ </param>
		/// <returns> ResourceBindLayout </returns>
		ResourceBindLayout CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlag flag) const
		{
            using UnderlyingType = std::underlying_type<ResourceBindFlag>::type;
			return CreateResourceBindLayout(viewTypes, static_cast<UnderlyingType>(flag));
		}

		/// <summary>
		/// ResourceBindLayoutを生成する
		/// </summary>
		/// <param name="viewTypes"> ビューをバインドする際のレイアウト </param>
		/// <param name="flags"> ResourceBindFlag組み合わせ </param>
		/// <returns> ResourceBindLayout </returns>
		ResourceBindLayout CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlags flags) const
		{
			return { viewTypes };
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
			return { size,clear,sampleCount };
		}

		/// <summary>
		/// GPUメモリの情報
		/// </summary>
		/// <returns></returns>
		std::pair<GpuMemoryInfo, GpuMemoryInfo> GetGpuMemoryInfo(void) const;

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

		Graphics(GpuEngine& gpuEngine, std::uint32_t bufferNum, std::size_t maxNum);

		Graphics();

#ifdef EUGENE_IMGUI
		ImGui_ImplVulkanH_Window* GetImguiWindow(void);
		vk::RenderPass GetRenderPass(void);
		vk::Framebuffer GetFrameBuffer(void);
#endif

		/// <summary>
		/// バックバッファ生成
		/// </summary>
		/// <param name="useVkformat"></param>
		/// <param name="size"></param>
		void CreateBackBuffer(vk::Format useVkformat, const glm::vec2& size);

		/// <summary>
		/// スワップチェイン生成
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		vk::Format CreateSwapChain(const glm::vec2& size);

		vk::Queue GetNextQueue()
		{
			return device_->getQueue(graphicFamilly_, nextQueueIdx_++);
		}

		void CreateInstance(void);
		void CreateDevice(void);
		void SetUpVma(void);

		/// <summary>
		/// ダイナミックローダー
		/// </summary>
		vk::DynamicLoader dl;

		/// <summary>
		/// vulkanのインスタンス
		/// </summary>
		vk::UniqueInstance instance_;

		/// <summary>
		/// 物理デバイス
		/// </summary>
		vk::PhysicalDevice physicalDevice_;

		/// <summary>
		/// vulkanのデバイス
		/// </summary>
		vk::UniqueDevice device_;

		/// <summary>
		/// メモリアロケーター
		/// </summary>
		vma::UniqueAllocator allocator_;

		/// <summary>
		/// サーフェスKHR
		/// </summary>
		vk::UniqueSurfaceKHR surfaceKhr_;

		/// <summary>
		/// ファミリーキュー
		/// </summary>
		std::uint32_t graphicFamilly_;

		/// <summary>
		/// バックバッファのインデックス
		/// </summary>
		std::uint32_t backBufferIdx_;

		/// <summary>
		/// キュー
		/// </summary>
		vk::Queue queue_;

		/// <summary>
		/// スワップチェイン
		/// </summary>
		vk::UniqueSwapchainKHR swapchain_;

		/// <summary>
		/// フェンス
		/// </summary>
		vk::UniqueFence fence_;

		/// <summary>
		/// バックバッファ
		/// </summary>
		std::vector<ImageResource, mi_stl_allocator<ImageResource>> buffers_;
		
		/// <summary>
		/// レンダーターゲットビュー
		/// </summary>
		RenderTargetViews renderTargetViews_;

		/// <summary>
		/// 画面が最小状態か？
		/// </summary>
		bool isMinimized;

		std::uint8_t multiSampleCount_;

		std::uint8_t nextQueueIdx_ = 0;

		/// <summary>
		/// バックバッファのフォーマット
		/// </summary>
		static inline Format backBufferFormat_;

#ifdef EUGENE_IMGUI

		/// <summary>
		/// Imgui用の画像データ
		/// </summary>
		struct ImguiImageData
		{
			/// <summary>
			/// ディスクリプタセット
			/// </summary>
			vk::UniqueDescriptorSet descriptorSet;

			/// <summary>
			/// イメージビュー
			/// </summary>
			vk::UniqueImageView imageView;
		};

		/// <summary>
		/// Imgui用ディスクリプタセット
		/// </summary>
		vk::UniqueDescriptorPool imguiDescriptorPool_;

		/// <summary>
		/// Imgui用レンダーパス
		/// </summary>
		vk::UniqueRenderPass imguiRenderPass_;

		/// <summary>
		/// Imgui用フレームバッファ
		/// </summary>
		std::vector<vk::UniqueFramebuffer, mi_stl_allocator<vk::UniqueFramebuffer>> imguiFrameBuffer_;

		/// <summary>
		/// Imgui用イメージデータ
		/// </summary>
		std::vector<ImguiImageData, mi_stl_allocator<ImguiImageData>> imageDatas_;

		/// <summary>
		/// Imgui用サンプラー
		/// </summary>
		vk::UniqueSampler imguiSampler_;

		std::uint64_t imguiImageMax_{ 1000ull };

#ifdef EUGENE_WINDOWS

		/// <summary>
		/// Imgui用の初期化処理
		/// </summary>
		/// <param name="useVkformat"> フォーマット </param>
		/// <param name="bufferNum"> バックバッファの数 </param>
		/// <param name="size"> バックバッファのサイズ </param>
		void InitImgui(vk::Format useVkformat, const uint32_t& bufferNum, const glm::vec2& size);

		/// <summary>
		/// Imgui用のフレームバッファを生成
		/// </summary>
		/// <param name="size"> フレームバッファのサイズ </param>
		void CreateImguiFrameBuffer(const glm::vec2& size);
#endif
#endif
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
		friend class Sampler;
	};

}