﻿#pragma once
#include <optional>
#include "GraphicsPipeline.h"
#include "GraphicsCommon.h"
#include "../ThirdParty/glm/glm/vec2.hpp"
#include "../ThirdParty/glm/glm/vec3.hpp"
#include "Sampler.h"
#include "../Utils/ArgsSpan.h"

#ifdef EUGENE_DX12
#include "DirectX12/Dx12Graphics.h"
#elif EUGENE_VULKAN
#include "Vulkan/VkGraphics.h"
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

		virtual void SetCameraPos(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up) = 0;

		virtual void SetCameraPos(const glm::mat4& mat) = 0;

		virtual void SetCameraProjection(float fov, float aspect, const glm::vec2& nearfar) = 0;

		virtual void SetCameraProjection(const glm::mat4& mat) = 0;
	protected:
		EffekseerWarpper();
	};
#endif

	/// <summary>
	/// グラフィックスの処理を行うクラス
	/// </summary>
	/*class Graphics
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
		virtual Pipeline* CreateGraphicsPipeline(
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>&  shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			bool useDepth = false,
			std::uint8_t sampleCount = 1
		) const = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="resourceBindLayout"></param>
		/// <param name="csShader"></param>
		/// <returns></returns>
		[[nodiscard]]
		virtual Pipeline* CreateComputePipeline(
			ResourceBindLayout& resourceBindLayout,
			const Shader& csShader
		) const = 0;

		[[nodiscard]]
		virtual ResourceBindLayout* CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlags flags) const = 0;

		[[nodiscard]]
		ResourceBindLayout* CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlag flag) const
		{
			return CreateResourceBindLayout(viewTypes, std::to_underlying(flag));
		}
		/// <summary>
		/// アップロードのためのバッファー(頂点、インデックス、定数)用のリソースを生成する
		/// </summary>
		/// <param name="size"> サイズ </param>
		/// <returns></returns>
		[[nodiscard]]
		virtual BufferResource* CreateUnloadableBufferResource(std::uint64_t size) const = 0;

		/// <summary>
		/// バッファー(頂点、インデックス、定数)用のリソースを生成する
		/// </summary>
		/// <param name="size"></param>
		/// <param name="isUnordered"> Unorderedで使用するか？ </param>
		/// <returns></returns>
		[[nodiscard]]
		virtual BufferResource* CreateBufferResource(std::uint64_t size, bool isUnordered = false) const = 0;

		[[nodiscard]]
		virtual BufferResource* CreateReadableBufferResource(std::uint64_t size, bool isUnordered = false) const = 0;

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
		/// レンダーターゲット用のImageResourceを生成
		/// </summary>
		/// <param name="size"></param>
		/// <param name="format"></param>
		/// <param name="arraySize"></param>
		/// <param name="mipLeveles"></param>
		/// <param name="sampleCount"></param>
		/// <param name="clearColor"></param>
		/// <returns></returns>
		[[nodiscard]]
		virtual ImageResource* CreateImageResource(
			const glm::ivec2& size,
			Format format,
			std::uint32_t arraySize = 1,
			std::uint8_t mipLeveles = 1,
			std::uint8_t sampleCount = 1,
			std::optional<std::span<float, 4>> clearColor = std::nullopt
		) = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="size"></param>
		/// <param name="clear"></param>
		/// <param name="sampleCount"></param>
		/// <returns></returns>
		[[nodiscard]]
		virtual ImageResource* CreateDepthResource(const glm::ivec2& size, float clear = 1, std::uint8_t sampleCount = 1) const = 0;

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
		virtual std::uint64_t GetNowBackBufferIndex(void) const = 0 ;

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
		/// <param name="viewTypes">  </param>
		/// <returns></returns>
		[[nodiscard]]
		virtual SamplerViews* CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const = 0;

		/// <summary>
		/// バックバッファをリサイズする
		/// </summary>
		/// <param name="size"></param>
		virtual void ResizeBackBuffer(const glm::vec2& size,void* window = nullptr);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="isFullScreen"></param>
		virtual void SetFullScreenFlag(bool isFullScreen);

		/// <summary>
		/// Gpuメモリの情報を取得します
		/// </summary>
		/// <param name=""> firstは専用メモリ、secondは共有メモリの情報 </param>
		/// <returns></returns>
		virtual std::pair<GpuMemoryInfo, GpuMemoryInfo> GetGpuMemoryInfo(void) const = 0;

		/// <summary>
		/// マルチサンプルアンチエイリアスで使える最大サンプル数を取得する
		/// </summary>
		/// <returns></returns>
		const std::uint8_t GetMaxMultiSampleCount() const;

#ifdef EUGENE_IMGUI

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
		/// Imguiで使用する画像をセットする
		/// </summary>
		/// <param name="imageResource"></param>
		/// <param name="index"></param>
		virtual void SetImguiImage(ImageResource& imageResource,std::uint64_t index = 0ull) = 0;
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

		/// <summary>
		/// マルチサンプルアンチエイリアスで使える最大サンプル数
		/// </summary>
		std::uint8_t multiSampleCount_;

		/// <summary>
		/// バックバッファのフォーマット
		/// </summary>
		static Format backBufferFormat_;

		friend class System;

#ifdef USE_IMGUI

		/// <summary>
		/// imguiで使用する画像の最大数
		/// </summary>
		const std::uint64_t imguiImageMax_{1000ull};
#endif
	};*/
}
