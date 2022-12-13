#pragma once
#include <cstdint>
#include <span>
#include "GraphicsCommon.h"
#include "../Math/Vector2.h"

namespace EugeneLib
{
	class Graphics;
	class RenderTargetViews;
	class DepthStencilViews;
	class VertexView;
	class GpuResource;
	class GraphicsPipeline;
	class ShaderResourceViews;
	class CommandList
	{
	public:
		virtual ~CommandList();
		virtual void Begin(void) = 0;
		virtual void End(void) = 0;
		virtual void SetGraphicsPipeline(GraphicsPipeline& gpipeline) = 0;
		virtual void SetPrimitiveType(PrimitiveType type) = 0;
		virtual void SetScissorrect(const Vector2I& leftTop, const Vector2I& rightBottom) = 0;
		virtual void SetViewPort(const Vector2& leftTop, const Vector2& size, float depthMin = 0.0f, float depthMax = 1.0f) = 0;
		
		virtual void SetVertexView(VertexView& view) = 0;

		virtual void SetShaderResourceView(ShaderResourceViews& views, size_t viewsIdx, size_t paramIdx) = 0;


		/*virtual void SetIndexView() = 0;*/
		virtual void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1) = 0;
		virtual void SetRenderTarget(RenderTargetViews& views, size_t idx = 0) = 0;
		virtual void SetRenderTarget(RenderTargetViews& views) = 0;
		virtual void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, size_t rtViewsIdx = 0, size_t dsViewsIdx = 0) = 0;
		virtual void SetRenderTarget(RenderTargetViews& views, DepthStencilViews& depthViews, size_t rtViewsIdx = 0) = 0;
		virtual void ClearRenderTarget(RenderTargetViews& views, std::span<float,4> color, size_t idx = 0) = 0;
		virtual void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) = 0;
		virtual void TransitionRenderTargetBegin(GpuResource& resource) = 0;
		virtual void TransitionRenderTargetEnd(GpuResource& resource) = 0;

		virtual void Copy(GpuResource& destination, GpuResource& source) = 0;

		virtual void CopyTexture(GpuResource& destination, GpuResource& source) = 0;

		virtual void* GetCommandList(void) const = 0;
	};

	CommandList* CreateCommandList(Graphics& graphics);

}
