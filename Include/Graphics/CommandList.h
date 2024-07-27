#pragma once
#include <cstdint>
#include <span>
#include <optional>
#include "GraphicsCommon.h"
#include "../ThirdParty/glm/glm/vec2.hpp"
#include "../ThirdParty/glm/glm/vec3.hpp"

#ifdef USE_IMGUI
struct ImDrawData;
#endif

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

	/// <summary>
	/// コマンドリストの処理を行うクラス
	/// </summary>
	class CommandList
	{
	public:

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~CommandList();

		/// <summary>
		/// コマンドリストを開始する
		/// </summary>
		/// <param name=""></param>
		virtual void Begin(void) = 0;

		/// <summary>
		/// コマンドリストを終了する
		/// </summary>
		/// <param name=""></param>
		virtual void End(void) = 0;

		/// <summary>
		/// グラフィックスパイプラインをセットする
		/// </summary>
		/// <param name="gpipeline"> グラフィックスパイプライン </param>
		virtual void SetGraphicsPipeline(Pipeline& gpipeline) = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="gpipeline"></param>
		virtual void SetComputePipeline(Pipeline& gpipeline) = 0;

		/// <summary>
		/// プリミティブタイプをセットする
		/// </summary>
		/// <param name="type"> プリミティブタイプ </param>
		virtual void SetPrimitiveType(PrimitiveType type) = 0;

		/// <summary>
		/// シザーレクトをセットする
		/// </summary>
		/// <param name="leftTop"> 左上 </param>
		/// <param name="rightBottom"> 右上 </param>
		virtual void SetScissorrect(const glm::ivec2& leftTop, const  glm::ivec2& rightBottom) = 0;

		/// <summary>
		/// ビューポートをセットする
		/// </summary>
		/// <param name="leftTop"> 左上 </param>
		/// <param name="size"> サイズ </param>
		/// <param name="depthMin"> 深度の最小値(デフォルト0.0f) </param>
		/// <param name="depthMax"> 深度の最大値(デフォルト1.0f) </param>
		virtual void SetViewPort(const glm::vec2& leftTop, const glm::vec2& size, float depthMin = 0.0f, float depthMax = 1.0f) = 0;
		
		/// <summary>
		/// 頂点ビューのセット
		/// </summary>
		/// <param name="view"> 頂点ビュー </param>
		virtual void SetVertexView(VertexView& view) = 0;

		/// <summary>
		/// インデックスビューをセットする
		/// </summary>
		/// <param name="view"> インデックスビュー </param>
		virtual void SetIndexView(IndexView& view) = 0;

		/// <summary>
		/// シェーダーリソースをセットする
		/// </summary>
		/// <param name="views"> シェーダーリソースのビュー </param>
		/// <param name="viewsIdx"> ビューのインデックス </param>
		/// <param name="paramIdx"> セットするインデックス </param>
		virtual void SetShaderResourceView(ShaderResourceViews& views, std::uint64_t paramIdx) = 0;

		/// <summary>
		/// コンピュートシェーダー用にシェーダーリソースをセットする
		/// </summary>
		/// <param name="views"></param>
		/// <param name="paramIdx"></param>
		virtual void SetShaderResourceViewComputeShader(ShaderResourceViews& views, std::uint64_t paramIdx) = 0;

		/// <summary>
		/// サンプラーをセットする
		/// </summary>
		/// <param name="views"></param>
		/// <param name="viewsIdx"></param>
		/// <param name="paramIdx"></param>
		virtual void SetSamplerView(SamplerViews& views, std::uint64_t paramIdx) = 0;

		/// <summary>
		/// 描画する
		/// </summary>
		/// <param name="vertexCount"> 頂点数 </param>
		/// <param name="instanceCount"> インスタンス数 </param>
		virtual void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1) = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="indexCount"></param>
		/// <param name="instanceNum"></param>
		/// <param name="offset"></param>
		virtual void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum = 1, std::uint32_t offset = 0) = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="count"></param>
		virtual void Dispatch(const glm::u32vec3& count) = 0;

		/// <summary>
		/// レンダーターゲットのセット(深度バッファあり)
		/// </summary>
		/// <param name="renderTargetViews"> レンダーターゲットビュー </param>
		/// <param name="depthViews"> デプスビュー(深度バッファ) </param>
		/// <param name="rtClear"> クリアカラー(ない場合はクリアしない) </param>
		/// <param name="rtRange"> レンダーターゲットビューの範囲(インデックスと数) </param>
		/// <param name="depthClear"> デプスのクリア値(ない場合クリアしない) </param>
		/// <param name="depthIndex"></param>
		virtual void SetRenderTarget(
			RenderTargetViews& renderTargetViews,
			DepthStencilViews& depthViews,
			std::optional<std::span<float, 4>> rtClear = {},
			std::pair<std::uint32_t, std::uint32_t> rtRange = { 0u, 0u },
			std::optional<float> depthClear = {},
			std::uint32_t depthIndex = 0u
		) = 0;

		/// <summary>
		/// レンダーターゲットをセット
		/// </summary>
		/// <param name="renderTargetViews"> レンダーターゲットビュー </param>
		/// <param name="rtClear"> クリアカラー(ない場合はクリアしない) </param>
		/// <param name="rtRange"> レンダーターゲットビューの範囲(インデックスと数) </param>
		virtual void SetRenderTarget(
			RenderTargetViews& renderTargetViews,
			std::optional<std::span<float, 4>> rtClear = {},
			std::pair<std::uint32_t, std::uint32_t> rtRange = { 0u, 1u }
			) = 0;

		/// <summary>
		/// レンダーターゲットの使用を開始できる状態にする
		/// </summary>
		/// <param name="resource"> レンダーターゲット </param>
		virtual void TransitionRenderTargetBegin(ImageResource& resource) = 0;

		/// <summary>
		/// レンダーターゲットの使用を終了の状態にする
		/// </summary>
		/// <param name="resource"> レンダーターゲット </param>
		virtual void TransitionRenderTargetEnd(ImageResource& resource) = 0;

		/// <summary>
		/// レンダーターゲットをシェーダ使用状態にする
		/// </summary>
		/// <param name="resource"></param>
		virtual void TransitionShaderResourceBegin(ImageResource& resource) = 0;

		/// <summary>
		/// レンダーターゲットをシェーダー使用状態から終了する
		/// </summary>
		/// <param name="resource"></param>
		virtual void TransitionShaderResourceEnd(ImageResource& resource) = 0;

		/// <summary>
		/// デプスを使用状態にする
		/// </summary>
		/// <param name="resource"></param>
		virtual void TransitionDepthBegin(ImageResource& resource) = 0;

		/// <summary>
		/// デプス使用状態から終了する
		/// </summary>
		/// <param name="resource"></param>
		virtual void TransitionDepthEnd(ImageResource& resource) = 0;

		/// <summary>
		/// UnorderedAccessでの使用状態を開始する
		/// </summary>
		/// <param name="resource"></param>
		virtual void TransitionUnorderedAccessBegin(BufferResource& resource) = 0;

		/// <summary>
		/// UnorderedAccessでの使用状態を終了する
		/// </summary>
		/// <param name="resource"></param>
		virtual void TransitionUnorderedAccessEnd(BufferResource& resource) = 0;

		/// <summary>
		/// テクスチャのコピーを行う
		/// </summary>
		/// <param name="dest"></param>
		/// <param name="src"></param>
		virtual void CopyTexture(ImageResource& dest, BufferResource& src) = 0;

		/// <summary>
		/// バッファ同士のコピーを行う
		/// </summary>
		/// <param name="dest"></param>
		/// <param name="src"></param>
		virtual void CopyBuffer(BufferResource& dest, BufferResource& src) = 0;

		/// <summary>
		/// MSAAのソースをResolveして結果を書き込みます
		/// </summary>
		/// <param name="dest"></param>
		/// <param name="src"></param>
		virtual void Resolve(ImageResource& dest, ImageResource& src) = 0;

		virtual void* GetCommandList(void) = 0;

#ifdef USE_IMGUI
		virtual void SetImguiCommand(ImDrawData* data, Graphics& graphics) const = 0;
#endif
	};
}
