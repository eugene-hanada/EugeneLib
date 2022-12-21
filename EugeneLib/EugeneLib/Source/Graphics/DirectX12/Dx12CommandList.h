#pragma once
#include <wrl.h>
#include "../../../Include/Graphics/CommandList.h"

struct ID3D12Device;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;

namespace EugeneLib
{
	class GpuResource;
	class Dx12CommandList :
		public CommandList
	{
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		Dx12CommandList(ID3D12Device* device);
		void* GetCommandList(void) const final;
	private:
		void Begin(void) final;
		void End(void) final;
		void SetGraphicsPipeline(GraphicsPipeline& gpipeline) final;
		void SetPrimitiveType(PrimitiveType type) final;
		void SetScissorrect(const Vector2I& leftTop, const Vector2I& rightBottom) final;
		void SetViewPort(const Vector2& leftTop, const Vector2& size, float depthMin = 0.0f, float depthMax = 1.0f);
		void SetVertexView(VertexView& view) final;

		void SetShaderResourceView(ShaderResourceViews& views, size_t viewsIdx, size_t paramIdx) final;

		void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1) final;

		void SetRenderTarget(RenderTargetViews& views, size_t idx = 0) final;
		void SetRenderTarget(RenderTargetViews& views) final;
		void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, size_t rtViewsIdx = 0, size_t dsViewsIdx = 0)final;
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color, size_t idx = 0) final;
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) final;
		void TransitionRenderTargetBegin(GpuResource& resource) final;
		void TransitionRenderTargetEnd(GpuResource& resource) final;

		void ClearDepth(DepthStencilViews& views, float clearValue = 1.0f, size_t idx = 0) final;

		void Copy(GpuResource& destination, GpuResource& source) final;
		void CopyTexture(GpuResource& destination, GpuResource& source) final;
		ComPtr< ID3D12CommandAllocator> cmdAllocator_;
		ComPtr< ID3D12GraphicsCommandList> cmdList_;

	};
}



