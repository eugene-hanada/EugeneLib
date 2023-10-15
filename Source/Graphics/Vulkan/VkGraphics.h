#pragma once
#include "../../../Include/Graphics/Graphics.h"
#include <vulkan/vulkan.hpp>
#include <memory>
#include <array>
#include <Windows.h>
#include "../../../Include/Graphics/ImageResource.h"
#include "../../../Include/Graphics/RenderTargetViews.h"

namespace Eugene
{

	class VkGraphics :
		public Graphics
	{
	public:
		static constexpr std::array<vk::Format, FormatMax> FormatToVkFormat{
			vk::Format::eUndefined,

			vk::Format::eUndefined,
			vk::Format::eR32G32B32A32Sfloat,
			vk::Format::eR32G32B32A32Uint,
			vk::Format::eR32G32B32A32Sint,
		
			vk::Format::eUndefined,
			vk::Format::eR32G32B32Sfloat,
			vk::Format::eR32G32B32Uint,
			vk::Format::eR32G32B32Sint,

			vk::Format::eUndefined,
			vk::Format::eR32G32Sfloat,
			vk::Format::eR32G32Uint,
			vk::Format::eR32G32Sint,

			vk::Format::eUndefined,
			vk::Format::eD32Sfloat,
			vk::Format::eR32Sfloat,
			vk::Format::eR32Uint,
			vk::Format::eR32Sint,

			vk::Format::eUndefined,
			vk::Format::eR16G16B16A16Sfloat,
			vk::Format::eR16G16B16A16Unorm,
			vk::Format::eR16G16B16A16Uint,
			vk::Format::eR16G16B16A16Snorm,
			vk::Format::eR16G16B16A16Sint,

			vk::Format::eUndefined,
			vk::Format::eR16G16Sfloat,
			vk::Format::eR16G16Unorm,
			vk::Format::eR16G16Uint,
			vk::Format::eR16G16Snorm,
			vk::Format::eR16G16Sint,

			vk::Format::eUndefined,
			vk::Format::eR16Sfloat,
			vk::Format::eR16Unorm,
			vk::Format::eR16Uint,
			vk::Format::eR16Snorm,
			vk::Format::eR16Sint,

			vk::Format::eUndefined,
			vk::Format::eR8G8B8A8Unorm,
			vk::Format::eR8G8B8A8Srgb,
			vk::Format::eR8G8B8A8Uint,
			vk::Format::eR8G8B8A8Snorm,
			vk::Format::eR8G8B8A8Sint,

			vk::Format::eBc1RgbaUnormBlock,
			vk::Format::eBc2UnormBlock,
			vk::Format::eBc3UnormBlock,
			vk::Format::eBc5UnormBlock,
			vk::Format::eBc7UnormBlock

		};

#ifdef USE_WINDOWS
		VkGraphics(HWND& hwnd,const Vector2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);
		vk::Format CreateSwapChain(const Eugene::Vector2& size);
#endif

		~VkGraphics();

		vk::UniqueDeviceMemory CreateMemory(vk::UniqueImage& image) const;
		vk::UniqueDeviceMemory CreateMemory(vk::Buffer& buffer, bool isDeviceLoacal = true, bool isHostVisible = false) const;
	private:
		GpuEngine* CreateGpuEngine(std::uint64_t maxSize) const final;
		CommandList* CreateCommandList(void) const final;
		GraphicsPipeline* CreateGraphicsPipeline(
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			bool useDepth = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan samplerLayout = SamplerSpan{}
		) const final;
		BufferResource* CreateUploadableBufferResource(std::uint64_t size) const final;
		BufferResource* CreateBufferResource(std::uint64_t size) const final;
		BufferResource* CreateBufferResource(Image& texture) const final;
		ImageResource* CreateImageResource(const TextureInfo& formatData) const final;
		ImageResource* CreateImageResource(const Vector2I& size, Format format, std::span<float, 4> color) final;
		ShaderResourceViews* CreateShaderResourceViews(std::uint64_t size) const final;
		ShaderResourceViews* CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const final;
		DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const final;
		RenderTargetViews* CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const final;
		VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const final;
		IndexView* CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const final;
		ImageResource& GetBackBufferResource(std::uint64_t idx) final;
		RenderTargetViews& GetViews(void) final;
		std::uint64_t GetNowBackBufferIndex(void) final;
		void Present(void) final;
		Sampler* CreateSampler(const SamplerLayout& layout) const final;
		SamplerViews* CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const final;
		SamplerViews* CreateSamplerViews(std::uint64_t size) const final;


		void CreateInstance(void);

		void CreateDevice(void);

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
		/// サーフェスKHR
		/// </summary>
		vk::UniqueSurfaceKHR surfaceKhr_;

		/// <summary>
		/// ファミリーキュー
		/// </summary>
		std::uint32_t graphicFamilly_;


		vk::Queue queue_;

		vk::UniqueSwapchainKHR swapchain_;

		std::shared_ptr<vk::UniqueSemaphore> semaphore_;

		std::shared_ptr<vk::UniqueFence> fence_;

		std::vector<std::unique_ptr<ImageResource>> buffers_;
		std::unique_ptr<RenderTargetViews> renderTargetViews_;

		// Graphics を介して継承されました
		GraphicsPipeline* CreateGraphicsPipeline(ResourceBindLayout& resourceBindLayout, const ArgsSpan<ShaderInputLayout>& layout, const ArgsSpan<ShaderPair>& shaders, const ArgsSpan<RendertargetLayout>& rendertarges, TopologyType topologyType, bool isCulling, bool useDepth) const final;
		ResourceBindLayout* CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes) const final;
		ImageResource* CreateDepthResource(const Vector2I& size, float clear) const final;
	};

}