#pragma once
#include "GraphicsPipeline.h"
#include "GraphicsCommon.h"
#include "../Math/Vector2.h"

namespace EugeneLib
{
	class System;
	class GpuEngine;
	class RenderTargetViews;
	class CommandList;
	class GpuResource;
	class Texture;
	class ShaderResourceViews;
	class VertexView;

	class Graphics
	{
	public:
		virtual ~Graphics();
		virtual GpuEngine* CreateGpuEngine(size_t maxSize) const = 0;
		virtual CommandList* CreateCommandList(void) const = 0;
		virtual GraphicsPipeline* CreateGraphicsPipeline(
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan samplerLayout = SamplerSpan{}
		) const = 0;
		
		virtual GpuResource* CreateUploadableResource(size_t size) const = 0;
		virtual GpuResource* CreateUploadableTextureResource(Texture& texture) const = 0;

		virtual GpuResource* CreateDefaultResource(size_t size) const = 0;
		virtual GpuResource* CreateTextureResource(const TextureInfo& formatData) const = 0;
		virtual GpuResource* CreateRenderTargetResource(const Vector2& size, Format format, const std::span<float, 4>& clearColor) const = 0;
		virtual GpuResource* CreateDepthResource(const Vector2& size, Format format, float clearValue = 1.0f) const = 0;

		virtual ShaderResourceViews* CreateShaderResourceViews(size_t size) const = 0;

		virtual RenderTargetViews* CreateRenderTargetViews(size_t size, bool isShaderVisible) const = 0;

		virtual VertexView* CreateVertexView(size_t size, size_t vertexNum, GpuResource& resource) const = 0;

		virtual GpuResource& GetBackBufferResource(void) = 0;
		virtual RenderTargetViews& GetViews(void) = 0;
		virtual size_t GetNowBackBufferIndex(void) = 0;
		virtual void Present(void) = 0;
	protected:
		Graphics();
		virtual GpuResource* CreateSwapChainResource(std::uint32_t idx) const = 0;
	};
}
