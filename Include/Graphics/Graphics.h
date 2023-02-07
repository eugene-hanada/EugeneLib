#pragma once
#include "GraphicsPipeline.h"
#include "GraphicsCommon.h"
#include "../Math/Vector2.h"
#include "Sampler.h"

namespace Eugene
{
	class System;
	class GpuEngine;
	class RenderTargetViews;
	class CommandList;
	class GpuResource;
	class Image;
	class ShaderResourceViews;
	class DepthStencilViews;
	class VertexView;
	class SamplerViews;

	/// <summary>
	/// グラフィックスの処理を行うクラス
	/// </summary>
	class Graphics
	{
	public:
		virtual ~Graphics();

		/// <summary>
		/// GpuEngineの生成
		/// </summary>
		/// <param name="maxSize"> 処理できるコマンドリストの最大数 </param>
		/// <returns> GpuEngineのポインタ </returns>
		virtual GpuEngine* CreateGpuEngine(size_t maxSize) const = 0;

		/// <summary>
		/// CommandListの生成
		/// </summary>
		/// <param name=""></param>
		/// <returns> CommandListのポインタ </returns>
		virtual CommandList* CreateCommandList(void) const = 0;

		/// <summary>
		/// GraphicsPipelineの生成
		/// </summary>
		/// <param name="layout"> シェーダーの入力レイアウト </param>
		/// <param name="shaders"> シェーダー </param>
		/// <param name="rendertarges"> レンダーターゲットの設定 </param>
		/// <param name="topologyType"> トポロジー設定 </param>
		/// <param name="isCulling"> カリングを行うか(デフォルトでは行わない) </param>
		/// <param name="shaderLayout"> シェーダーで使う定数バッファ等の情報 </param>
		/// <param name="samplerLayout"> サンプラーの情報 </param>
		/// <returns></returns>
		virtual GraphicsPipeline* CreateGraphicsPipeline(
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan samplerLayout = SamplerSpan{}
		) const = 0;
		
		/// <summary>
		/// アップロード用のGpuResourceの生成
		/// </summary>
		/// <param name="size"> リソースのサイズ </param>
		/// <returns> GpuResourceのポインタ </returns>
		virtual GpuResource* CreateUploadableResource(size_t size) const = 0;

		/// <summary>
		/// テクスチャのアップロード用GpuResourceの生成
		/// </summary>
		/// <param name="texture"> テクスチャ </param>
		/// <returns> GpuResourceのポインタ </returns>
		virtual GpuResource* CreateUploadableTextureResource(Image& texture) const = 0;

		virtual GpuResource* CreateUploadableResource(const Vector2& size, Format format) const = 0;

		/// <summary>
		/// GpuResourceの生成
		/// </summary>
		/// <param name="size"> リソースのサイズ </param>
		/// <returns> GpuResourceのポインタ </returns>
		virtual GpuResource* CreateDefaultResource(size_t size) const = 0;

		/// <summary>
		/// テクスチャ用GpuResourceの生成
		/// </summary>
		/// <param name="formatData"> テクスチャのフォーマット </param>
		/// <returns> GpuResourceのポインタ </returns>
		virtual GpuResource* CreateTextureResource(const TextureInfo& formatData) const = 0;

		/// <summary>
		/// レンダーターゲット用GpuResourceの生成
		/// </summary>
		/// <param name="size"> レンダーターゲットのサイズ </param>
		/// <param name="format"> フォーマット </param>
		/// <param name="clearColor"> 初期化カラー </param>
		/// <returns> GpuResourceのポインタ </returns>
		virtual GpuResource* CreateRenderTargetResource(const Vector2& size, Format format, const std::span<float, 4>& clearColor) const = 0;

		/// <summary>
		/// 深度バッファ用GpuResourceの生成
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <param name="format"> フォーマット </param>
		/// <param name="clearValue"> 初期化の値 </param>
		/// <returns> GpuResourceのポインタ </returns>
		virtual GpuResource* CreateDepthResource(const Vector2& size, Format format, float clearValue = 1.0f) const = 0;

		/// <summary>
		/// ShaderResourceViewsの生成
		/// </summary>
		/// <param name="size"> Viewの数 </param>
		/// <returns> ShaderResourceViewsのポインタ </returns>
		virtual ShaderResourceViews* CreateShaderResourceViews(size_t size) const = 0;

		/// <summary>
		/// RenderTargetViewsの生成
		/// </summary>
		/// <param name="size"> Viewの数 </param>
		/// <param name="isShaderVisible"> シェーダーに見える状態にするか? </param>
		/// <returns> RenderTargetViewsのポインタ </returns>
		virtual RenderTargetViews* CreateRenderTargetViews(size_t size, bool isShaderVisible) const = 0;

		/// <summary>
		/// DepthStencilViewsの生成
		/// </summary>
		/// <param name="size"> Viewの数 </param>
		/// <returns> DepthStencilViewsのポインタ </returns>
		virtual DepthStencilViews* CreateDepthStencilViews(size_t size) const = 0;

		/// <summary>
		/// VertexViewの生成
		/// </summary>
		/// <param name="size"> 1頂点当たりのサイズ </param>
		/// <param name="vertexNum"> 頂点数 </param>
		/// <param name="resource"> 頂点のGpuResource </param>
		/// <returns> VertexViewのポインタ </returns>
		virtual VertexView* CreateVertexView(size_t size, size_t vertexNum, GpuResource& resource) const = 0;

		/// <summary>
		/// バックバッファのレンダーターゲットのGpuResourceを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> GpuResourceの参照 </returns>
		GpuResource& GetBackBufferResource(void);
		
		/// <summary>
		/// バックバッファのレンダーターゲットをインデックス指定でGpuResourceを取得する
		/// </summary>
		/// <param name="idx"> インデックス </param>
		/// <returns></returns>
		virtual GpuResource& GetBackBufferResource(size_t idx) = 0;

		/// <summary>
		/// バックバッファで使用するRenderTargetViewsの取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> RenderTargetViews </returns>
		virtual RenderTargetViews& GetViews(void) = 0;

		/// <summary>
		/// 現在のバックバッファのインデックスを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> インデックス </returns>
		virtual size_t GetNowBackBufferIndex(void) = 0;
		virtual void Present(void) = 0;

		virtual Sampler* CreateSampler(const SamplerLayout& layout) const = 0;

		virtual SamplerViews* CreateSamplerViews(size_t size) const = 0;
#ifdef USE_IMGUI
		virtual void ImguiNewFrame(void) const = 0;
#endif
	protected:
		Graphics();

		/// <summary>
		/// スワップチェイン用のGpuResourceの生成
		/// </summary>
		/// <param name="idx"> バックバッファのインデックス </param>
		/// <returns> GpuResourceのポインタ </returns>
		virtual GpuResource* CreateSwapChainResource(std::uint32_t idx) const = 0;
	};
}
