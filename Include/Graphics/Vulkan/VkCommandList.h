#pragma once
#include "../GraphicsCommon.h"
#include <vulkan/vulkan.hpp>
#include <optional>
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

		/// <summary>
		/// コマンド開始処理
		/// </summary>
		void Begin(void);

		/// <summary>
		/// コマンド終了処理
		/// </summary>
		void End(void);

		/// <summary>
		/// グラフィックスパイプラインをセット
		/// </summary>
		/// <param name="gpipeline"></param>
		void SetGraphicsPipeline(Pipeline& gpipeline);

		/// <summary>
		/// プリミティブタイプをセットする
		/// </summary>
		/// <param name="type"></param>
		void SetPrimitiveType(PrimitiveType type) ;

		/// <summary>
		/// シザーレクトをセットする
		/// </summary>
		/// <param name="leftTop"> 左上 </param>
		/// <param name="rightBottom"> 右上 </param>
		void SetScissorrect(const glm::ivec2& leftTop, const glm::ivec2& rightBottom);

		/// <summary>
		/// ビューポートをセットする
		/// </summary>
		/// <param name="leftTop"> 左上 </param>
		/// <param name="size"> サイズ </param>
		/// <param name="depthMin"> 深度最小値 </param>
		/// <param name="depthMax"> 振動最大値 </param>
		void SetViewPort(const glm::vec2& leftTop, const glm::vec2& size, float depthMin = 0.0f, float depthMax = 1.0f) ;

		/// <summary>
		/// 頂点ビューをセットする
		/// </summary>
		/// <param name="view"></param>
		void SetVertexView(VertexView& view) ;

		/// <summary>
		/// インデックスビューをセットする
		/// </summary>
		/// <param name="view"></param>
		void SetIndexView(IndexView& view) ;

		/// <summary>
		/// sシェーダリソースビューをセットする
		/// </summary>
		/// <param name="views"> インデックス </param>
		/// <param name="paramIdx"> パラメーターインデックス </param>
		void SetShaderResourceView(ShaderResourceViews& views, std::uint64_t paramIdx) ;

		/// <summary>
		/// サンプラービューをセットする
		/// </summary>
		/// <param name="views"> サンプラービュー </param>
		/// <param name="paramIdx"> パラメーターインデックス </param>
		void SetSamplerView(SamplerViews& views, std::uint64_t paramIdx) ;

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="vertexCount"> 頂点数 </param>
		/// <param name="instanceCount"> インスタンス数 </param>
		void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1) ;

		/// <summary>
		/// インデックス付き描画
		/// </summary>
		/// <param name="indexCount"> インデックス数</param>
		/// <param name="instanceNum"> インスタンス数 </param>
		/// <param name="offset"> オフセット </param>
		void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum, std::uint32_t offset) ;
		
		/// <summary>
		/// レンダーターゲットをセットする
		/// </summary>
		/// <param name="renderTargetViews"> レンダーターゲットのビュー </param>
		/// <param name="depthViews">　デプスステンシルのビュー　</param>
		/// <param name="rtClear"> レンダーターゲットのクリアカラー </param>
		/// <param name="rtRange"> ビューの範囲 </param>
		/// <param name="depthClear"> デプスステンシルのクリアカラー </param>
		/// <param name="depthIndex"> デプスステンシルのビューのインデックス </param>
		void SetRenderTarget(
			RenderTargetViews& renderTargetViews,
			DepthStencilViews& depthViews,
			std::optional<std::span<float, 4>> rtClear = {},
			std::pair<std::uint32_t, std::uint32_t> rtRange = { 0u, 1u },
			std::optional<float> depthClear = {},
			std::uint32_t depthIndex = 0u
		) ;

		/// <summary>
		/// レンダーターゲットをセットする
		/// </summary>
		/// <param name="renderTargetViews"></param>
		/// <param name="rtClear"> レンダーターゲットのクリアカラー </param>
		/// <param name="rtRange"> ビューの範囲 </param>
		void SetRenderTarget(
			RenderTargetViews& renderTargetViews,
			std::optional<std::span<float, 4>> rtClear = {},
			std::pair<std::uint32_t, std::uint32_t> rtRange = { 0u, 1u }
		) ;

		/// <summary>
		/// レンダーターゲット状態として開始する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionRenderTargetBegin(ImageResource& resource) ;

		/// <summary>
		/// レンダーターゲット状態を終了する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionRenderTargetEnd(ImageResource& resource) ;

		/// <summary>
		/// シェーダリソース状態として開始する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionShaderResourceBegin(ImageResource& resource) ;

		/// <summary>
		/// シェーダリソース状態を終了する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionShaderResourceEnd(ImageResource& resource) ;

		/// <summary>
		/// デプスステンシル状態として開始する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionDepthBegin(ImageResource& resource) ;

		/// <summary>
		/// デプスステンシル状態を終了する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionDepthEnd(ImageResource& resource) ;

		/// <summary>
		/// バッファからテクスチャをコピーする
		/// </summary>
		/// <param name="dest"></param>
		/// <param name="src"></param>
		void CopyTexture(ImageResource& dest, BufferResource& src) ;

		/// <summary>
		/// バッファをコピーする
		/// </summary>
		/// <param name="dest"></param>
		/// <param name="src"></param>
		void CopyBuffer(BufferResource& dest, BufferResource& src) ;

		/// <summary>
		/// MSAAのResolve処理をする
		/// </summary>
		/// <param name="dest"></param>
		/// <param name="src"></param>
		void Resolve(ImageResource& dest, ImageResource& src) ;

		/// <summary>
		/// APIのコマンドリストを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> コマンドリストのポインタ </returns>
		void* GetCommandList(void) noexcept;

#ifdef USE_IMGUI
		/// <summary>
		/// ImGui用のコマンドをセットする
		/// </summary>
		/// <param name="data"></param>
		void SetImguiCommand(ImDrawData* data) const;
#endif

		/// <summary>
		/// コンピュート用のパイプラインをセットする
		/// </summary>
		/// <param name="gpipeline"></param>
		void SetComputePipeline(Pipeline& gpipeline);

		/// <summary>
		/// コンピュートシェーダにシェーダリソースをセットする
		/// </summary>
		/// <param name="views"></param>
		/// <param name="paramIdx"></param>
		void SetShaderResourceViewComputeShader(ShaderResourceViews& views, std::uint64_t paramIdx);

		/// <summary>
		/// コンピュートシェーダをディスパッチ
		/// </summary>
		/// <param name="count"></param>
		void Dispatch(const glm::u32vec3& count);

		/// <summary>
		/// Unorderedとして使用を開始する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionUnorderedAccessBegin(BufferResource& resource);

		/// <summary>
		/// Unorderedとして使用を終端する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionUnorderedAccessEnd(BufferResource& resource);

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			commandBuffer_.reset();
			commandPool_.reset();
			isRendering_ = false;
			nowLayout_.reset();
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="cmdList"></param>
		CommandList(CommandList&& cmdList) noexcept:
			commandPool_{std::move(cmdList.commandPool_)}, commandBuffer_{std::move(cmdList.commandBuffer_)},
			isRendering_{cmdList.isRendering_}, nowLayout_{std::move(cmdList.nowLayout_)}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="cmdList"></param>
		/// <returns></returns>
		CommandList& operator=(CommandList&& cmdList) noexcept
		{
			commandPool_ = std::move(cmdList.commandPool_);
			commandBuffer_ = std::move(cmdList.commandBuffer_);
			isRendering_ = cmdList.isRendering_;
			nowLayout_ = std::move(cmdList.nowLayout_);
		}


		CommandList(const CommandList&) = delete;
		CommandList& operator=(const CommandList&) = delete;
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

