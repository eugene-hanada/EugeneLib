#pragma once
#include "GraphicsPipeline.h"
#include "GraphicsCommon.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "Sampler.h"
#include "../Common/ArgsSpan.h"

#ifdef USE_EFFEKSEER
namespace Effekseer
{
	template<class T>
	class RefPtr;

	class Manager;
}
#endif

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

#ifdef USE_EFFEKSEER

	/// <summary>
	/// Effekseer用ラップクラスの基底
	/// </summary>
	class EffekseerWarpper
	{
	public:
		virtual ~EffekseerWarpper();

		/// <summary>
		/// 更新処理
		/// </summary>
		/// <param name="delta"></param>
		virtual void Update(float delta) = 0;

		/// <summary>
		/// 描画処理
		/// </summary>
		/// <param name="cmdList"></param>
		virtual void Draw(CommandList& cmdList) = 0;

		/// <summary>
		/// マネージャーの取得
		/// </summary>
		/// <returns></returns>
		virtual Effekseer::RefPtr<Effekseer::Manager>& GetManager()& = 0;

		virtual void SetCameraPos(const Vector3& eye, const Vector3& at, const Vector3& up) = 0;

		virtual void SetCameraProjection(float fov, float aspect, const Eugene::Vector2& nearfar) = 0;
	protected:
		EffekseerWarpper();
	};
#endif

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
		[[nodiscard]]
		virtual GpuEngine* CreateGpuEngine(std::uint64_t maxSize) const = 0;

		/// <summary>
		/// CommandListの生成
		/// </summary>
		/// <param name=""></param>
		/// <returns> CommandListのポインタ </returns>
		[[nodiscard]]
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
		[[nodiscard]]
		virtual GraphicsPipeline* CreateGraphicsPipeline(
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			bool useDepth = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan samplerLayout = SamplerSpan{}
		) const = 0;
		
		/// <summary>
		/// グラフィックスパイプラインクラスを生成する
		/// </summary>
		/// <param name="resourceBindLayout"> シェーダへのリソースバインドを設定への参照 </param>
		/// <param name="layout"> シェーダ入力のレイアウト </param>
		/// <param name="shaders"> 各シェーダ(頂点シェーダ、ピクセルシェーダ...など) </param>
		/// <param name="rendertarges"> レンダーターゲットの設定 </param>
		/// <param name="topologyType"> トポロジータイプ </param>
		/// <param name="isCulling"> バックカリングを使用するか? </param>
		/// <param name="useDepth">　デプスバッファを使用するか?　</param>
		/// <returns></returns>
		[[nodiscard]]
		virtual GraphicsPipeline* CreateGraphicsPipeline(
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>&  shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			bool useDepth = false
		) const = 0;

		[[nodiscard]]
		virtual ResourceBindLayout* CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes) const = 0;

		/// <summary>
		/// アップロードのためのバッファー(頂点、インデックス、定数)用のリソースを生成する
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <returns></returns>
		[[nodiscard]]
		virtual BufferResource* CreateUploadableBufferResource(std::uint64_t size) const = 0;

		/// <summary>
		/// バッファー(頂点、インデックス、定数)用のリソースを生成する
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <returns></returns>
		[[nodiscard]]
		virtual BufferResource* CreateBufferResource(std::uint64_t size) const = 0;

		/// <summary>
		/// テクスチャアップロード用バッファーリソースを生成する
		/// </summary>
		/// <param name="texture"> Image </param>
		/// <returns></returns>
		[[nodiscard]]
		virtual BufferResource* CreateBufferResource(Image& texture) const = 0;


		/// <summary>
		/// 
		/// </summary>
		/// <param name="formatData"></param>
		/// <returns></returns>
		[[nodiscard]]
		virtual ImageResource* CreateImageResource(const TextureInfo& formatData) const = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="size"></param>
		/// <param name="format"></param>
		/// <param name="clearColor"></param>
		/// <returns></returns>
		[[nodiscard]]
		virtual ImageResource* CreateImageResource(const Vector2I& size, Format format, std::span<float, 4> clearColor) = 0;

		[[nodiscard]]
		virtual ImageResource* CreateDepthResource(const Vector2I& size, float clear) const = 0;

		/// <summary>
		/// ShaderResourceViewsの生成
		/// </summary>
		/// <param name="size"> Viewの数 </param>
		/// <returns> ShaderResourceViewsのポインタ </returns>
		[[nodiscard,deprecated("CreateShaderResourceViews(const ArgsSpan<Bind>& )を使用してください")]]
		virtual ShaderResourceViews* CreateShaderResourceViews(std::uint64_t size) const = 0;

		/// <summary>
		/// ShaderResourceViewsの生成
		/// </summary>
		/// <param name="viewTypes"> Viewの情報 </param>
		/// <returns> ShaderResourceViewsのポインタ </returns>
		[[nodiscard]]
		virtual ShaderResourceViews* CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const = 0;

		/// <summary>
		/// RenderTargetViewsの生成
		/// </summary>
		/// <param name="size"> Viewの数 </param>
		/// <param name="isShaderVisible"> シェーダーに見える状態にするか? </param>
		/// <returns> RenderTargetViewsのポインタ </returns>
		[[nodiscard]]
		virtual RenderTargetViews* CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const = 0;

		/// <summary>
		/// DepthStencilViewsの生成
		/// </summary>
		/// <param name="size"> Viewの数 </param>
		/// <returns> DepthStencilViewsのポインタ </returns>
		[[nodiscard]]
		virtual DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const = 0;

		/// <summary>
		/// VertexViewの生成
		/// </summary>
		/// <param name="size"> 1頂点当たりのサイズ </param>
		/// <param name="vertexNum"> 頂点数 </param>
		/// <param name="resource"> 頂点のGpuResource </param>
		/// <returns> VertexViewのポインタ </returns>
		[[nodiscard]]
		virtual VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const = 0;

		/// <summary>
		/// インデックスビュー生成
		/// </summary>
		/// <param name="size"> 一つ当たりのバイト数 </param>
		/// <param name="num"> 数 </param>
		/// <param name="format"> フォーマット </param>
		/// <param name="resource"> リソース </param>
		/// <returns> IndexViewのポインタ </returns>
		[[nodiscard]]
		virtual IndexView* CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const = 0;

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
		[[nodiscard]]
		virtual Sampler* CreateSampler(const SamplerLayout& layout) const = 0;

		/// <summary>
		/// サンプラービューを作成する
		/// </summary>
		/// <param name="size"> ビューの数 </param>
		/// <returns></returns>
		[[nodiscard, deprecated("CreateSamplerViews(const ArgsSpan<Bind>& )を使用してください")]]
		virtual SamplerViews* CreateSamplerViews(std::uint64_t size) const = 0;

		/// <summary>
		/// サンプラービューを作成する
		/// </summary>
		/// <param name="viewTypes">  </param>
		/// <returns></returns>
		[[nodiscard]]
		virtual SamplerViews* CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const = 0;

		/// <summary>
		/// バックバッファをリサイズする
		/// </summary>
		/// <param name="size"></param>
		virtual void ResizeBackBuffer(const Vector2& size);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="isFullScreen"></param>
		virtual void SetFullScreenFlag(bool isFullScreen);
#ifdef USE_IMGUI

		/// <summary>
		/// imgui用のフレーム開始の処理をする
		/// </summary>
		/// <param name=""></param>
		virtual void ImguiNewFrame(void) const = 0;

		/// <summary>
		/// Imgui用のシェーダーリソースからテクスチャ用のIDを取得する
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		virtual void* GetImguiImageID(std::uint64_t index) const = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual ShaderResourceViews& GetImguiShaderResourceView(void) & = 0;
#endif

#ifdef USE_EFFEKSEER

		[[nodiscard]]
		virtual EffekseerWarpper* CreateEffekseerWarpper(
			GpuEngine& gpuEngine,
			Format rtFormat,
			std::uint32_t rtNum,
			Format depthFormat = Format::NON,
			bool reverseDepth = false,
			std::uint64_t maxNumm = 8000
		) const = 0;
#endif

	protected:
		Graphics();
	};
}
