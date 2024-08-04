#pragma once
#include <optional>
#include <span>
#include <wrl.h>
#include <d3d12.h>
#include "../GraphicsCommon.h"
#include "../../../ThirdParty/glm/glm/vec2.hpp"
#include "../../../ThirdParty/glm/glm/vec3.hpp"


struct ImDrawData;


namespace Eugene
{
	class Graphics;
	class RenderTargetViews;
	class DepthStencilViews;
	class VertexView;
	class IndexView;

	class GpuResource;
	class BufferResource;
	class ImageResource;

	class Pipeline;
	class ShaderResourceViews;
	class SamplerViews;


	class CommandList
	{
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		void* GetCommandList(void);
		CommandList(CommandList&& cmdList) noexcept :
			cmdList_{cmdList.cmdList_}, cmdAllocator_{cmdList.cmdAllocator_}
		{
			cmdList.Final();
		}

		CommandList& operator=(CommandList&& cmdList) noexcept
		{
			cmdList_ = cmdList.cmdList_;
			cmdAllocator_ = cmdList.cmdAllocator_;
			cmdList.Final();
		}

		void Final() noexcept
		{
			cmdList_.Reset();
			cmdAllocator_.Reset();
		}

		// 開始処理
		void Begin(void);

		// 終了処理
		void End(void);

		// グラフィックスパイプラインをセットする
		void SetGraphicsPipeline(Pipeline& gpipeline);

		void SetComputePipeline(Pipeline& gpipeline);

		// プリミティブタイプをセットする
		void SetPrimitiveType(PrimitiveType type);

		// シザーレクトをセットする
		void SetScissorrect(const glm::ivec2& leftTop, const glm::ivec2& rightBottom);

		// ビューポートをセットする
		void SetViewPort(const glm::vec2& leftTop, const glm::vec2& size, float depthMin = 0.0f, float depthMax = 1.0f);

		// 頂点ビューをセットする
		void SetVertexView(VertexView& view);

		// インデックスビューをセットする
		void SetIndexView(IndexView& view);

		// シェーダリソースセット系 //

		// 指定のパラメータインデックスに指定のビューのインデックスを先頭にセットする
		void SetShaderResourceView(ShaderResourceViews& views, std::uint64_t paramIdx);

		void SetShaderResourceViewComputeShader(ShaderResourceViews& views, std::uint64_t paramIdx);

		// 指定のパラメータインデックスに指定のサンプラービューのインデックスを先頭にセットする
		void SetSamplerView(SamplerViews& views, std::uint64_t paramIdx);

		// 描画系 //

		// 描画
		void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1);

		// インデックス付きで描画
		void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum = 1, std::uint32_t offset = 0);

		void Dispatch(const glm::u32vec3& count);

		// レンダーターゲットセット系 //

		//// 指定のインデックスのビューをセットする
		//void SetRenderTarget(RenderTargetViews& views, std::uint64_t idx = 0) final;

		//// 指定の範囲のインデックスのビューをセットする
		//void SetRenderTarget(RenderTargetViews& views, std::uint64_t startIdx, std::uint64_t endIdx) final;

		//// ビューにあるすべてをセットする
		//void SetRenderTarget(RenderTargetViews& views) final;

		//// 指定のインデックスのデプスとレンダーターゲットのビューをセットする
		//void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, std::uint64_t rtViewsIdx = 0, std::uint64_t dsViewsIdx = 0)final;

		//// クリア系 //

		//// 
		//void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color, std::uint64_t idx = 0) final;
		//void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) final;

		//void ClearDepth(DepthStencilViews& views, float clearValue = 1.0f, std::uint64_t idx = 0) final;

		// バリア遷移系 //

		// レンダーターゲットを開始状態にする
		void TransitionRenderTargetBegin(ImageResource& resource);

		// レンダーターゲットを終了状態にする(Commonとか)
		void TransitionRenderTargetEnd(ImageResource& resource);

		// シェーダー使用状態にする
		void TransitionShaderResourceBegin(ImageResource& resource);

		// シェーダー使用状態から終了する
		void TransitionShaderResourceEnd(ImageResource& resource);

		// デプス使用状態にする
		void TransitionDepthBegin(ImageResource& resource);

		// デプス使用状態から終了する
		void TransitionDepthEnd(ImageResource& resource);

		void TransitionUnorderedAccessBegin(BufferResource& resource);

		void TransitionUnorderedAccessEnd(BufferResource& resource);

		void CopyTexture(ImageResource& dest, BufferResource& src);

		void Resolve(ImageResource& dest, ImageResource& src);

		// CommandList を介して継承されました
		virtual void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, std::optional<std::span<float, 4>> rtClear, std::pair<std::uint32_t, std::uint32_t> rtRange, std::optional<float> depthClear, std::uint32_t depthIndex);

		virtual void SetRenderTarget(RenderTargetViews& renderTargetViews, std::optional<std::span<float, 4>> rtClear, std::pair<std::uint32_t, std::uint32_t> rtRange);

		virtual void CopyBuffer(BufferResource& dest, BufferResource& src);

#ifdef USE_IMGUI
		void SetImguiCommand(ImDrawData* data, Graphics& graphics) const;
#endif
	private:
		CommandList();
		CommandList(const CommandList&) = delete;
		CommandList& operator=(const CommandList&) = delete;

		
		/// <summary>
		/// コマンドアロケーター
		/// </summary>
		ComPtr< ID3D12CommandAllocator> cmdAllocator_;

		/// <summary>
		/// コマンドリスト
		/// </summary>
		ComPtr< ID3D12GraphicsCommandList> cmdList_;

		friend class Graphics;
	};
}



