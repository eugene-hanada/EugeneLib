﻿#pragma once
#include "../../../Include/Graphics/Graphics.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/ThirdParty/VulkanMemoryAllocator-Hpp/include/vk_mem_alloc.hpp"
#include <memory>
#include <array>

#ifdef USE_WINDOWS
#include <Windows.h>
#else USE_ANDROID
struct android_app;
#endif

#include "../../../Include/Graphics/ImageResource.h"
#include "../../../Include/Graphics/RenderTargetViews.h"

#ifdef USE_IMGUI
struct ImGui_ImplVulkanH_Window;
#endif

namespace Eugene
{
#ifdef USE_EFFEKSEER
	class EffekseerWarpper;
#endif

	class VkGraphics :
		public Graphics
	{
	public:
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

#ifdef USE_WINDOWS
		VkGraphics(HWND& hwnd,const glm::vec2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);
#else  USE_ANDROID
        VkGraphics(android_app* app,const glm::vec2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);
#endif
		~VkGraphics();

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

		/// <summary>
		/// Image用デバイスメモリを生成する
		/// </summary>
		/// <param name="image"></param>
		/// <returns></returns>
		vk::UniqueDeviceMemory CreateMemory(vk::UniqueImage& image) const;

		/// <summary>
		/// buffer用デバイスメモリを生成する
		/// </summary>
		/// <param name="buffer"></param>
		/// <param name="isDeviceLoacal"></param>
		/// <param name="isHostVisible"></param>
		/// <returns></returns>
		vk::UniqueDeviceMemory CreateMemory(vk::Buffer& buffer, bool isDeviceLoacal = true, bool isHostVisible = false) const;

#ifdef USE_IMGUI
		ImGui_ImplVulkanH_Window* GetImguiWindow(void);
		vk::RenderPass GetRenderPass(void);
		vk::Framebuffer GetFrameBuffer(void);
#endif
	private:
		GpuEngine* CreateGpuEngine(std::uint64_t maxSize) const final;
		CommandList* CreateCommandList(void) const final;
		BufferResource* CreateUploadableBufferResource(std::uint64_t size) const final;
		BufferResource* CreateBufferResource(std::uint64_t size) const final;
		BufferResource* CreateBufferResource(Image& texture) const final;
		ImageResource* CreateImageResource(const TextureInfo& formatData) const final;

		ImageResource* CreateImageResource(
			const glm::ivec2& size,
			Format format, 
			std::uint32_t arraySize = 1,
			std::uint8_t mipLeveles = 1,
			std::uint8_t sampleCount = 1,
			std::optional<std::span<float, 4>> clearColor = std::nullopt
			) final;
		ShaderResourceViews* CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const final;
		DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const final;
		RenderTargetViews* CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const final;
		VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const final;
		IndexView* CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const final;
		ImageResource& GetBackBufferResource(std::uint64_t idx) final;
		RenderTargetViews& GetViews(void) final;
		std::uint64_t GetNowBackBufferIndex(void) const final;
		void Present(void) final;
		Sampler* CreateSampler(const SamplerLayout& layout) const final;
		SamplerViews* CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const final;
		void ResizeBackBuffer(const glm::vec2& size, void* window = nullptr) final;
		void SetFullScreenFlag(bool isFullScreen) final;
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
		ResourceBindLayout* CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes) const final;
		ImageResource* CreateDepthResource(const glm::ivec2& size, float clear = 1, std::uint8_t sampleCount = 1) const final;
		std::pair<GpuMemoryInfo, GpuMemoryInfo> GetGpuMemoryInfo(void) const final;
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
		std::vector<std::unique_ptr<ImageResource>> buffers_;
		
		/// <summary>
		/// レンダーターゲットビュー
		/// </summary>
		std::unique_ptr<RenderTargetViews> renderTargetViews_;

		/// <summary>
		/// 画面が最小状態か？
		/// </summary>
		bool isMinimized;

#ifdef USE_EFFEKSEER
		// Graphics を介して継承されました
		EffekseerWarpper* CreateEffekseerWarpper(GpuEngine& gpuEngine, Format rtFormat, std::uint32_t rtNum, Format depthFormat = Format::NON,
			bool reverseDepth = false,
			std::uint64_t maxNumm = 8000) const final;

		vk::UniqueCommandPool effekseerPool_;
#endif

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


		void ImguiNewFrame(void) const final;
		void* GetImguiImageID(std::uint64_t index) const final;
		void SetImguiImage(ImageResource& imageResource, std::uint64_t index = 0ull) final;
#ifdef USE_WINDOWS

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



};

}