﻿#pragma once
#include <wrl.h>
#include "../../../Include/Graphics/CommandList.h"

struct ID3D12Device;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;



namespace Eugene
{
	class GpuResource;
	class Dx12CommandList :
		public CommandList
	{
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		Dx12CommandList(ID3D12Device* device);
		void* GetCommandList(void) const final;
	private:

		// 開始処理
		void Begin(void) final;

		// 終了処理
		void End(void) final;

		// グラフィックスパイプラインをセットする
		void SetGraphicsPipeline(GraphicsPipeline& gpipeline) final;

		// プリミティブタイプをセットする
		void SetPrimitiveType(PrimitiveType type) final;

		// シザーレクトをセットする
		void SetScissorrect(const Vector2I& leftTop, const Vector2I& rightBottom) final;

		// ビューポートをセットする
		void SetViewPort(const Vector2& leftTop, const Vector2& size, float depthMin = 0.0f, float depthMax = 1.0f);

		// 頂点ビューをセットする
		void SetVertexView(VertexView& view) final;

		// インデックスビューをセットする
		void SetIndexView(IndexView& view) final;

		// シェーダリソースセット系 //

		// 指定のパラメータインデックスに指定のビューのインデックスを先頭にセットする
		void SetShaderResourceView(ShaderResourceViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx) final;

		// 指定のパラメータインデックスに指定のサンプラービューのインデックスを先頭にセットする
		void SetSamplerView(SamplerViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx) final;

		// 描画系 //

		// 描画
		void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1) final;

		// インデックス付きで描画
		void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum = 1, std::uint32_t offset = 0) final;

		// レンダーターゲットセット系 //

		// 指定のインデックスのビューをセットする
		void SetRenderTarget(RenderTargetViews& views, std::uint64_t idx = 0) final;

		// 指定の範囲のインデックスのビューをセットする
		void SetRenderTarget(RenderTargetViews& views, std::uint64_t startIdx, std::uint64_t endIdx) final;

		// ビューにあるすべてをセットする
		void SetRenderTarget(RenderTargetViews& views) final;

		// 指定のインデックスのデプスとレンダーターゲットのビューをセットする
		void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, std::uint64_t rtViewsIdx = 0, std::uint64_t dsViewsIdx = 0)final;

		// クリア系 //

		// 
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color, std::uint64_t idx = 0) final;
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) final;

		void ClearDepth(DepthStencilViews& views, float clearValue = 1.0f, std::uint64_t idx = 0) final;

		// バリア遷移系 //

		// レンダーターゲットを開始状態にする
		void TransitionRenderTargetBegin(ImageResource& resource) final;

		// レンダーターゲットを終了状態にする(Commonとか)
		void TransitionRenderTargetEnd(ImageResource& resource) final;

		// シェーダー使用状態にする
		void TransitionShaderResourceBegin(ImageResource& resource) final;

		// シェーダー使用状態から終了する
		void TransitionShaderResourceEnd(ImageResource& resource) final;

		// デプス使用状態にする
		void TransitionDepthBegin(ImageResource& resource) final;

		// デプス使用状態から終了する
		void TransitionDepthEnd(ImageResource& resource) final;

		// コピー系 //

		void Copy(GpuResource& destination, GpuResource& source) final;
		void CopyTexture(GpuResource& destination, GpuResource& source) final;

		void CopyTexture(ImageResource& dest, BufferResource& src) final;

#ifdef USE_IMGUI
		void SetImguiCommand(ImDrawData* data, Graphics& graphics) const final;
#endif

		ComPtr< ID3D12CommandAllocator> cmdAllocator_;
		ComPtr< ID3D12GraphicsCommandList> cmdList_;

	};
}



