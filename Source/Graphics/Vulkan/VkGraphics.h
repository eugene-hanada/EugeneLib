#pragma once
#include "../../../Include/Graphics/Graphics.h"

namespace Eugene
{
    class VkGraphics :
        public Graphics
    {
    public:
        VkGraphics(const Vector2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum);
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
        ShaderResourceViews* CreateShaderResourceViews(std::uint64_t size) const final;
        DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const final;
        VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const final;
        IndexView* CreateIndexView(std::uint32_t size, Format format, BufferResource& resource) const final;
        ImageResource& GetBackBufferResource(std::uint64_t idx) final;
        RenderTargetViews& GetViews(void) final;
        std::uint64_t GetNowBackBufferIndex(void) final;
        void Present(void) final;
        Sampler* CreateSampler(const SamplerLayout& layout) const final;
        SamplerViews* CreateSamplerViews(std::uint64_t size) const final;
    };

}