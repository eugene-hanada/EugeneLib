#pragma once
#include <cstdint>
#include <span>

namespace EugeneLib
{
	class Graphics;
	class RenderTargetViews;
	class DepthStencilViews;
	class VertexView;
	class GpuResource;
	class CommandList
	{
	public:
		virtual ~CommandList();
		virtual void Begin(void) = 0;
		virtual void End(void) = 0;
		virtual void SetVertexView(VertexView& view) = 0;
		/*virtual void SetIndexView() = 0;*/
		/*virtual void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount) = 0;*/
		virtual void SetRenderTarget(RenderTargetViews& views, size_t idx = 0) = 0;
		virtual void SetRenderTarget(RenderTargetViews& views) = 0;
		virtual void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, size_t rtViewsIdx = 0, size_t dsViewsIdx = 0) = 0;
		virtual void SetRenderTarget(RenderTargetViews& views, DepthStencilViews& depthViews, size_t rtViewsIdx = 0) = 0;
		virtual void ClearRenderTarget(RenderTargetViews& views, std::span<float,4> color, size_t idx = 0) = 0;
		virtual void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) = 0;
		virtual void TransitionRenderTargetBegin(GpuResource& resource) = 0;
		virtual void TransitionRenderTargetEnd(GpuResource& resource) = 0;

		virtual void Copy(GpuResource& destination, GpuResource& source) = 0;
		virtual void* GetCommandList(void) const = 0;
	};

	CommandList* CreateCommandList(Graphics& graphics);

}
