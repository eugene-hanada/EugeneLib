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
		virtual void SetGraphicsPipeline(GraphicsPipeline& gpipeline) = 0;

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
		virtual void SetScissorrect(const Vector2I& leftTop, const Vector2I& rightBottom) = 0;

		/// <summary>
		/// ビューポートをセットする
		/// </summary>
		/// <param name="leftTop"> 左上 </param>
		/// <param name="size"> サイズ </param>
		/// <param name="depthMin"> 深度の最小値(デフォルト0.0f) </param>
		/// <param name="depthMax"> 深度の最大値(デフォルト1.0f) </param>
		virtual void SetViewPort(const Vector2& leftTop, const Vector2& size, float depthMin = 0.0f, float depthMax = 1.0f) = 0;
		
		/// <summary>
		/// 頂点ビューのセット
		/// </summary>
		/// <param name="view"> 頂点ビュー </param>
		virtual void SetVertexView(VertexView& view) = 0;

		/// <summary>
		/// シェーダーリソースをセットする
		/// </summary>
		/// <param name="views"> シェーダーリソースのビュー </param>
		/// <param name="viewsIdx"> ビューのインデックス </param>
		/// <param name="paramIdx"> セットするインデックス </param>
		virtual void SetShaderResourceView(ShaderResourceViews& views, size_t viewsIdx, size_t paramIdx) = 0;

		/// <summary>
		/// 描画する
		/// </summary>
		/// <param name="vertexCount"> 頂点数 </param>
		/// <param name="instanceCount"> インスタンス数 </param>
		virtual void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1) = 0;

		/// <summary>
		/// レンダーターゲットをセットする
		/// </summary>
		/// <param name="views"> レンダーターゲットのビュー </param>
		/// <param name="idx"> ビューのインデックス </param>
		virtual void SetRenderTarget(RenderTargetViews& views, size_t idx = 0) = 0;

		/// <summary>
		/// レンダーターゲットをセットする(RenderTargetViewsにあるものすべてをセットする)
		/// </summary>
		/// <param name="views"> レンダーターゲットビュー </param>
		virtual void SetRenderTarget(RenderTargetViews& views) = 0;

		/// <summary>
		/// レンダーターゲットをセットする(深度バッファもセットする)
		/// </summary>
		/// <param name="renderTargetViews"> レンダーターゲットビュー </param>
		/// <param name="depthViews"> デプスステンシルビュー </param>
		/// <param name="rtViewsIdx"> レンダーターゲットビューのインデックス </param>
		/// <param name="dsViewsIdx"> デプスステンシルビュー のインデックス </param>
		virtual void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, size_t rtViewsIdx = 0, size_t dsViewsIdx = 0) = 0;

		/// <summary>
		/// レンダーターゲットをクリアする
		/// </summary>
		/// <param name="views"> レンダーターゲットビュー </param>
		/// <param name="color"> クリアするカラー </param>
		/// <param name="idx"> レンダーターゲットビューのインデックス </param>
		virtual void ClearRenderTarget(RenderTargetViews& views, std::span<float,4> color, size_t idx = 0) = 0;

		/// <summary>
		///  レンダーターゲットをクリアする(RenderTargetViewsにあるものすべてクリアする)
		/// </summary>
		/// <param name="views"> レンダーターゲットビュー </param>
		/// <param name="color"> クリアするカラー </param>
		virtual void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) = 0;

		/// <summary>
		/// レンダーターゲットの使用を開始できる状態にする
		/// </summary>
		/// <param name="resource"> レンダーターゲット </param>
		virtual void TransitionRenderTargetBegin(GpuResource& resource) = 0;

		/// <summary>
		/// レンダーターゲットの使用を終了の状態にする
		/// </summary>
		/// <param name="resource"> レンダーターゲット </param>
		virtual void TransitionRenderTargetEnd(GpuResource& resource) = 0;

		/// <summary>
		/// 深度バッファをクリアする
		/// </summary>
		/// <param name="views"> デプスステンシルビュー </param>
		/// <param name="clearValue"> クリアする値 </param>
		/// <param name="idx"> デプスステンシルビューのインデックス </param>
		virtual void ClearDepth(DepthStencilViews& views, float clearValue = 1.0f, size_t idx = 0) = 0;

		/// <summary>
		/// リソースをコピーする
		/// </summary>
		/// <param name="destination"> コピー先 </param>
		/// <param name="source"> コピー元 </param>
		virtual void Copy(GpuResource& destination, GpuResource& source) = 0;

		/// <summary>
		/// テクスチャのコピーをする
		/// </summary>
		/// <param name="destination"> コピー先 </param>
		/// <param name="source"> コピー元 </param>
		virtual void CopyTexture(GpuResource& destination, GpuResource& source) = 0;

		virtual void* GetCommandList(void) const = 0;
	};
}
