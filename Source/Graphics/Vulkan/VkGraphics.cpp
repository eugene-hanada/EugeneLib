#include "VkGraphics.h"

Eugene::VkGraphics::VkGraphics(const Vector2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum)
{
}

Eugene::VkGraphics::~VkGraphics()
{
}

Eugene::GpuEngine* Eugene::VkGraphics::CreateGpuEngine(std::uint64_t maxSize) const
{
    return nullptr;
}

Eugene::CommandList* Eugene::VkGraphics::CreateCommandList(void) const
{
    return nullptr;
}

Eugene::GraphicsPipeline* Eugene::VkGraphics::CreateGraphicsPipeline(ShaderInputSpan layout, ShaderTypePaisrSpan shaders, RenderTargetSpan rendertarges, TopologyType topologyType, bool isCulling, ShaderLayoutSpan shaderLayout, SamplerSpan samplerLayout) const
{
    return nullptr;
}

Eugene::BufferResource* Eugene::VkGraphics::CreateUploadableBufferResource(std::uint64_t size) const
{
    return nullptr;
}

Eugene::BufferResource* Eugene::VkGraphics::CreateBufferResource(std::uint64_t size) const
{
    return nullptr;
}

Eugene::BufferResource* Eugene::VkGraphics::CreateBufferResource(Image& texture) const
{
    return nullptr;
}

Eugene::ImageResource* Eugene::VkGraphics::CreateImageResource(const TextureInfo& formatData) const
{
    return nullptr;
}

Eugene::ShaderResourceViews* Eugene::VkGraphics::CreateShaderResourceViews(std::uint64_t size) const
{
    return nullptr;
}

Eugene::DepthStencilViews* Eugene::VkGraphics::CreateDepthStencilViews(std::uint64_t size) const
{
    return nullptr;
}

Eugene::VertexView* Eugene::VkGraphics::CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const
{
    return nullptr;
}

Eugene::IndexView* Eugene::VkGraphics::CreateIndexView(std::uint32_t size, Format format, BufferResource& resource) const
{
    return nullptr;
}

Eugene::ImageResource& Eugene::VkGraphics::GetBackBufferResource(std::uint64_t idx)
{
    // TODO: return ステートメントをここに挿入します
}

Eugene::RenderTargetViews& Eugene::VkGraphics::GetViews(void)
{
    // TODO: return ステートメントをここに挿入します
}

std::uint64_t Eugene::VkGraphics::GetNowBackBufferIndex(void)
{
    return std::uint64_t();
}

void Eugene::VkGraphics::Present(void)
{
}

Eugene::Sampler* Eugene::VkGraphics::CreateSampler(const SamplerLayout& layout) const
{
    return nullptr;
}

Eugene::SamplerViews* Eugene::VkGraphics::CreateSamplerViews(std::uint64_t size) const
{
    return nullptr;
}
