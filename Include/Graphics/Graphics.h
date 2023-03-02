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

	class BufferResource;
	class ImageResource;

	class Image;
	class ShaderResourceViews;
	class DepthStencilViews;
	class VertexView;
	class IndexView;
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
		virtual GpuEngine* CreateGpuEngine(std::uint64_t maxSize) const = 0;

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
		/// アップロードのためのバッファー(頂点、インデックス、定数)用のリソースを生成する
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <returns></returns>
		virtual BufferResource* CreateUploadableBufferResource(std::uint64_t size) const = 0;

		/// <summary>
		/// バッファー(頂点、インデックス、定数)用のリソースを生成する
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <returns></returns>
		virtual BufferResource* CreateBufferResource(std::uint64_t size) const = 0;

		/// <summary>
		/// テクスチャアップロード用バッファーリソースを生成する
		/// </summary>
		/// <param name="texture"> Image </param>
		/// <returns></returns>
		virtual BufferResource* CreateBufferResource(Image& texture) const = 0;


		/// <summary>
		/// 
		/// </summary>
		/// <param name="formatData"></param>
		/// <returns></returns>
		virtual ImageResource* CreateImageResource(const TextureInfo& formatData) const = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="size"></param>
		/// <param name="format"></param>
		/// <param name="clearColor"></param>
		/// <returns></returns>
		virtual ImageResource* CreateImageResource(const Vector2I& size, Format format, std::span<float, 4> clearColor) = 0;

		/// <summary>
		/// ShaderResourceViewsの生成
		/// </summary>
		/// <param name="size"> Viewの数 </param>
		/// <returns> ShaderResourceViewsのポインタ </returns>
		virtual ShaderResourceViews* CreateShaderResourceViews(std::uint64_t size) const = 0;

		/// <summary>
		/// RenderTargetViewsの生成
		/// </summary>
		/// <param name="size"> Viewの数 </param>
		/// <param name="isShaderVisible"> シェーダーに見える状態にするか? </param>
		/// <returns> RenderTargetViewsのポインタ </returns>
		virtual RenderTargetViews* CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const = 0;

		/// <summary>
		/// DepthStencilViewsの生成
		/// </summary>
		/// <param name="size"> Viewの数 </param>
		/// <returns> DepthStencilViewsのポインタ </returns>
		virtual DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const = 0;

		/// <summary>
		/// VertexViewの生成
		/// </summary>
		/// <param name="size"> 1頂点当たりのサイズ </param>
		/// <param name="vertexNum"> 頂点数 </param>
		/// <param name="resource"> 頂点のGpuResource </param>
		/// <returns> VertexViewのポインタ </returns>
		virtual VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const = 0;

		/// <summary>
		/// IndexViewの生成
		/// </summary>
		/// <param name="size"></param>
		/// <param name="resource"></param>
		/// <returns></returns>
		virtual IndexView* CreateIndexView(std::uint32_t size, Format format, BufferResource& resource) const = 0;

		/// <summary>
		/// バックバッファのレンダーターゲットのGpuResourceを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> GpuResourceの参照 </returns>
		ImageResource& GetBackBufferResource(void);
		
		/// <summary>
		/// バックバッファのレンダーターゲットをインデックス指定でGpuResourceを取得する
		/// </summary>
		/// <param name="idx"> インデックス </param>
		/// <returns></returns>
		virtual ImageResource& GetBackBufferResource(std::uint64_t idx) = 0;

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
		virtual std::uint64_t GetNowBackBufferIndex(void) = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		virtual void Present(void) = 0;

		/// <summary>
		/// サンプラーを作成する
		/// </summary>
		/// <param name="layout"> サンプラーの設定 </param>
		/// <returns></returns>
		virtual Sampler* CreateSampler(const SamplerLayout& layout) const = 0;

		/// <summary>
		/// サンプラービューを作成する
		/// </summary>
		/// <param name="size"> ビューの数 </param>
		/// <returns></returns>
		virtual SamplerViews* CreateSamplerViews(std::uint64_t size) const = 0;
#ifdef USE_IMGUI
		virtual void ImguiNewFrame(void) const = 0;
#endif
	protected:
		Graphics();
	};
}
