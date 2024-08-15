#pragma once
#include "../GraphicsCommon.h"
#include <vulkan/vulkan.hpp>
#include "../../../ThirdParty/glm/glm/vec2.hpp"
#include "../../../ThirdParty/glm/glm/vec3.hpp"
#ifdef USE_IMGUI
struct ImDrawData;
#endif

namespace Eugene
{
	class BufferResource;
	class ImageResource;
	class Pipeline;
	class RenderTargetViews;
	class VertexView;
	class IndexView;
	class ShaderResourceViews;
	class SamplerViews;
	class DepthStencilViews;


	/// <summary>
	/// コマンドリストクラスをvulkan実装版
	/// </summary>
	class CommandList
	{
	public:

		// CommandList を介して継承されました
		void Begin(void);
		void End(void) ;
		void SetGraphicsPipeline(Pipeline& gpipeline);
		void SetPrimitiveType(PrimitiveType type) ;
		void SetScissorrect(const glm::ivec2& leftTop, const glm::ivec2& rightBottom) ;
		void SetViewPort(const glm::vec2& leftTop, const glm::vec2& size, float depthMin = 0.0f, float depthMax = 1.0f) ;
		void SetVertexView(VertexView& view) ;
		void SetIndexView(IndexView& view) ;
		void SetShaderResourceView(ShaderResourceViews& viewsviewsIdx, std::uint64_t paramIdx) ;
		void SetSamplerView(SamplerViews& views, std::uint64_t paramIdx) ;
		void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1) ;
		void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum, std::uint32_t offset) ;
		
		void SetRenderTarget(
			RenderTargetViews& renderTargetViews,
			DepthStencilViews& depthViews,
			std::optional<std::span<float, 4>> rtClear = {},
			std::pair<std::uint32_t, std::uint32_t> rtRange = { 0u, 1u },
			std::optional<float> depthClear = {},
			std::uint32_t depthIndex = 0u
		) ;

		
		void SetRenderTarget(
			RenderTargetViews& renderTargetViews,
			std::optional<std::span<float, 4>> rtClear = {},
			std::pair<std::uint32_t, std::uint32_t> rtRange = { 0u, 1u }
		) ;

		void TransitionRenderTargetBegin(ImageResource& resource) ;
		void TransitionRenderTargetEnd(ImageResource& resource) ;
		void TransitionShaderResourceBegin(ImageResource& resource) ;
		void TransitionShaderResourceEnd(ImageResource& resource) ;
		void TransitionDepthBegin(ImageResource& resource) ;
		void TransitionDepthEnd(ImageResource& resource) ;
		void CopyTexture(ImageResource& dest, BufferResource& src) ;
		void CopyBuffer(BufferResource& dest, BufferResource& src) ;
		void Resolve(ImageResource& dest, ImageResource& src) ;
		void* GetCommandList(void) ;

#ifdef USE_IMGUI
		// CommandList を介して継承されました
		void SetImguiCommand(ImDrawData* data) const;
#endif

		// CommandList を介して継承されました
		void SetComputePipeline(Pipeline& gpipeline);
		void SetShaderResourceViewComputeShader(ShaderResourceViews& views, std::uint64_t paramIdx);
		void Dispatch(const glm::u32vec3& count);
		void TransitionUnorderedAccessBegin(BufferResource& resource);
		void TransitionUnorderedAccessEnd(BufferResource& resource);

		void Final() noexcept
		{
			commandBuffer_.reset();
			commandPool_.reset();
			isRendering_ = false;
			nowLayout_.reset();
		}
	private:
		CommandList();

		/// <summary>
		/// コマンドプール
		/// </summary>
		vk::UniqueCommandPool commandPool_;



		/// <summary>
		/// コマンドバッファ
		/// </summary>
		vk::UniqueCommandBuffer commandBuffer_;

		/// <summary>
		/// レンダリング開始しているか？
		/// </summary>
		bool isRendering_;

		/// <summary>
		/// 現在のパイプラインのレイアウト
		/// </summary>
		std::shared_ptr<vk::UniquePipelineLayout> nowLayout_;

		friend class Graphics;


	};
}

