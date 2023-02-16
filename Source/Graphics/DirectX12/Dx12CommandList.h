﻿#pragma once
#include <wrl.h>
#include "../../../Include/Graphics/CommandList.h"

struct ID3D12Device;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;



namespace Eugene
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
		void SetIndexView(IndexView& view) final;
		void SetShaderResourceView(ShaderResourceViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx) final;
		void SetSamplerView(SamplerViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx) final;
		void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1) final;
		void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum = 1, std::uint32_t offset = 0) final;
		void SetRenderTarget(RenderTargetViews& views, std::uint64_t idx = 0) final;
		void SetRenderTarget(RenderTargetViews& views) final;
		void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, std::uint64_t rtViewsIdx = 0, std::uint64_t dsViewsIdx = 0)final;
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color, std::uint64_t idx = 0) final;
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) final;
		void TransitionRenderTargetBegin(ImageResource& resource) final;
		void TransitionRenderTargetEnd(ImageResource& resource) final;

		void ClearDepth(DepthStencilViews& views, float clearValue = 1.0f, std::uint64_t idx = 0) final;

		void Copy(GpuResource& destination, GpuResource& source) final;
		void CopyTexture(GpuResource& destination, GpuResource& source) final;

		void CopyTexture(ImageResource& dest, BufferResource& src) final;

#ifdef USE_IMGUI
		void SetImguiCommand(ImDrawData* data, Graphics& graphics) const final;
#endif

		ComPtr< ID3D12CommandAllocator> cmdAllocator_;
		ComPtr< ID3D12GraphicsCommandList> cmdList_;

	};
}



