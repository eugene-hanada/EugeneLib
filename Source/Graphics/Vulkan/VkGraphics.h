#pragma once
#include "../../../Include/Graphics/Graphics.h"
#include <vulkan/vulkan.hpp>
#include <memory>
#include "../../../Include/Graphics/ImageResource.h"
#include "../../../Include/Graphics/RenderTargetViews.h"

namespace Eugene
{

    class VkGraphics :
        public Graphics
    {
    public:
#ifdef BUILD_WINDOWS
        VkGraphics(HWND& hwnd,const Vector2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);
        void CreateSwapChain(const Eugene::Vector2& size);
#endif

        ~VkGraphics();
    private:
        GpuEngine* CreateGpuEngine(std::uint64_t maxSize) const final;
        CommandList* CreateCommandList(void) const final;
        GraphicsPipeline* CreateGraphicsPipeline(
            ShaderInputSpan layout,
            ShaderTypePaisrSpan  shaders,
            RenderTargetSpan rendertarges,
            TopologyType topologyType = TopologyType::Triangle,
            bool isCulling = false,
            ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
            SamplerSpan samplerLayout = SamplerSpan{}
        ) const final;
        BufferResource* CreateUploadableBufferResource(std::uint64_t size) const final;
        BufferResource* CreateBufferResource(std::uint64_t size) const final;
        BufferResource* CreateBufferResource(Image& texture) const final;
        ImageResource* CreateImageResource(const TextureInfo& formatData) const final;
        ImageResource* CreateImageResource(const Vector2I& size, Format format, std::span<float, 4> color) final;
        ShaderResourceViews* CreateShaderResourceViews(std::uint64_t size) const final;
        DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const final;
        RenderTargetViews* CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const final;
        VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const final;
        IndexView* CreateIndexView(std::uint32_t size, Format format, BufferResource& resource) const final;
        ImageResource& GetBackBufferResource(std::uint64_t idx) final;
        RenderTargetViews& GetViews(void) final;
        std::uint64_t GetNowBackBufferIndex(void) final;
        void Present(void) final;
        Sampler* CreateSampler(const SamplerLayout& layout) const final;
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
    };

}