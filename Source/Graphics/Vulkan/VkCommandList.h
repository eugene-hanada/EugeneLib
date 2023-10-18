#pragma once
#include "../../../Include/Graphics/CommandList.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	/// <summary>
	/// コマンドリストクラスをvulkan実装版
	/// </summary>
	class VkCommandList :
		public CommandList
	{
	public:
		VkCommandList(const vk::Device& device, std::uint32_t familyIndex);
	private:
		// CommandList を介して継承されました
		void Begin(void) final;
		void End(void) final;
		void SetGraphicsPipeline(GraphicsPipeline& gpipeline) final;
		void SetPrimitiveType(PrimitiveType type) final;
		void SetScissorrect(const Vector2I& leftTop, const Vector2I& rightBottom) final;
		void SetViewPort(const Vector2& leftTop, const Vector2& size, float depthMin, float depthMax) final;
		void SetVertexView(VertexView& view) final;
		void SetIndexView(IndexView& view) final;
		void SetShaderResourceView(ShaderResourceViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx) final;
		void SetSamplerView(SamplerViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx) final;
		void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount) final;
		void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum, std::uint32_t offset) final;
		void SetRenderTarget(RenderTargetViews& views, std::uint64_t idx) final;
		void SetRenderTarget(RenderTargetViews& views, std::uint64_t startIdx, std::uint64_t endIdx) final;
		void SetRenderTarget(RenderTargetViews& views) final;
		void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, std::uint64_t rtViewsIdx, std::uint64_t dsViewsIdx) final;
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color, std::uint64_t idx) final;
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) final;
		void TransitionRenderTargetBegin(ImageResource& resource) final;
		void TransitionRenderTargetEnd(ImageResource& resource) final;
		void TransitionShaderResourceBegin(ImageResource& resource) final;
		void TransitionShaderResourceEnd(ImageResource& resource) final;
		void TransitionDepthBegin(ImageResource& resource) final;
		void TransitionDepthEnd(ImageResource& resource) final;
		void ClearDepth(DepthStencilViews& views, float clearValue, std::uint64_t idx) final;
		void Copy(GpuResource& destination, GpuResource& source) final;
		void CopyTexture(GpuResource& destination, GpuResource& source) final;
		void CopyTexture(ImageResource& dest, BufferResource& src) final;
		void CopyBuffer(BufferResource& dest, BufferResource& src) final;
		void* GetCommandList(void) final;


		/// <summary>
		/// コマンドプール
		/// </summary>
		vk::UniqueCommandPool commandPool_;

		/// <summary>
		/// コマンドバッファ
		/// </summary>
		vk::UniqueCommandBuffer commandBuffer_;

		/// <summary>
		/// DynamicRendering用の設定
		/// </summary>
		vk::RenderingInfo renderingInfo_;

		/// <summary>
		/// レンダリング開始しているか？
		/// </summary>
		bool isRendering_;

		vk::PipelineLayout* nowLayout_;

	};
}

