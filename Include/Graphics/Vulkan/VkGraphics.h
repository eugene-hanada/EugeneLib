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



#ifdef USE_IMGUI
struct ImGui_ImplVulkanH_Window;
#endif

namespace Eugene
{
	class Graphics :
		public DynamicSingleton<Graphics>
	{
	public:

		static GpuEngine Create(std::uint32_t bufferNum = 2, std::uint64_t maxNum = 100)
		{
			GpuEngine gpuEngine;
			new Graphics{ gpuEngine, bufferNum, maxNum };
			return gpuEngine;
		}

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

		GpuEngine CreateGpuEngine(std::size_t initSize) const
		{
			return { initSize };
		}


		CommandList CreateCommandList(void) const
		{
			return {};
		}

		BufferResource CreateReadableBufferResource(std::uint64_t size, bool isUnordered = false) const
		{
			return { size,isUnordered,GpuResourceType::ReadBack };
		}
		BufferResource CreateBufferResource(std::uint64_t size, bool isUnordered = false) const
		{
			return { size, isUnordered, GpuResourceType::Default };
		}

		BufferResource CreateBufferResource(Image& texture) const
		{
			return { texture };
		}

		BufferResource CreateUnloadableBufferResource(std::uint64_t size) const
		{
			return { size, false,GpuResourceType::Upload };
		}

		ImageResource CreateImageResource(const TextureInfo& formatData) const
		{
			return { formatData };
		}

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

		ShaderResourceViews CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const
		{
			return { viewTypes };
		}

		DepthStencilViews CreateDepthStencilViews(std::uint32_t size) const
		{
			return { size };
		}

		RenderTargetViews CreateRenderTargetViews(std::uint32_t size, bool isShaderVisible) const
		{
			return { size };
		}

		ImageResource& GetBackBufferResource(std::uint64_t idx)
		{
			return buffers_[idx];
		}

		RenderTargetViews& GetViews(void) noexcept
		{
			return renderTargetViews_;
		}
		std::uint64_t GetNowBackBufferIndex(void) const noexcept
		{
			return backBufferIdx_;
		}


		ImageResource& GetBackBufferResource() noexcept
		{
			return GetBackBufferResource(GetNowBackBufferIndex());
		}

		void Present(void);
		Sampler CreateSampler(const SamplerLayout& layout) const
		{
			return { layout };
		}

		SamplerViews CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const
		{
			return { viewTypes };
		}

		void ResizeBackBuffer(const glm::vec2& size, void* window = nullptr);
		void SetFullScreenFlag(bool isFullScreen);

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

		// Graphics を介して継承されました
		Pipeline CreateComputePipeline(ResourceBindLayout& resourceBindLayout, const Shader& csShader) const
		{
			return { resourceBindLayout, csShader };
		}


		ResourceBindLayout CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlag flag) const
		{
			return CreateResourceBindLayout(viewTypes, std::to_underlying(flag));
		}
		ResourceBindLayout CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlags flags) const
		{
			return { viewTypes };
		}




		ImageResource CreateDepthResource(const glm::ivec2& size, float clear = 1, std::uint8_t sampleCount = 1) const
		{
			return { size,clear,sampleCount };
		}

		std::pair<GpuMemoryInfo, GpuMemoryInfo> GetGpuMemoryInfo(void) const;

#ifdef USE_IMGUI
		void ImguiNewFrame(void) const;
		void* GetImguiImageID(std::uint64_t index) const;
		void SetImguiImage(ImageResource& imageResource, std::uint64_t index = 0ull);
#endif	

private:

#ifdef EUGENE_WINDOWS
		Graphics(GpuEngine& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);
#else  EUGENE_ANDROID
		Graphics(GpuEngine& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);
#endif

#ifdef USE_IMGUI
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
		/// バックバッファのインデックス
		/// </summary>
		std::uint32_t backBufferIdx_;

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
		std::vector<ImageResource> buffers_;
		
		/// <summary>
		/// レンダーターゲットビュー
		/// </summary>
		RenderTargetViews renderTargetViews_;

		/// <summary>
		/// 画面が最小状態か？
		/// </summary>
		bool isMinimized;

		std::uint8_t multiSampleCount_;

		std::uint32_t nextQueueIdx_ = 0;

		/// <summary>
		/// バックバッファのフォーマット
		/// </summary>
		static inline Format backBufferFormat_;

#ifdef USE_IMGUI

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
		std::vector<vk::UniqueFramebuffer> imguiFrameBuffer_;

		/// <summary>
		/// Imgui用イメージデータ
		/// </summary>
		std::vector<ImguiImageData> imageDatas_;

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