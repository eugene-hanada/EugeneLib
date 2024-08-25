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

	class BufferResource;
	class ImageResource;

	class Pipeline;
	class ShaderResourceViews;
	class SamplerViews;

	/// <summary>
	/// コマンドリスト
	/// </summary>
	class CommandList
	{
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		CommandList() = default;

		/// <summary>
		/// API側のコマンドリストを取得する
		/// </summary>
		/// <returns> コマンドリストのポインタ </returns>
		void* GetCommandList(void) noexcept;

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="cmdList"></param>
		CommandList(CommandList&& cmdList) noexcept :
			cmdList_{std::move(cmdList.cmdList_)}, cmdAllocator_{std::move(cmdList.cmdAllocator_)}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="cmdList"></param>
		/// <returns></returns>
		CommandList& operator=(CommandList&& cmdList) noexcept
		{
			cmdList_ = std::move(cmdList.cmdList_);
			cmdAllocator_ = std::move(cmdList.cmdAllocator_);
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			cmdList_.Reset();
			cmdAllocator_.Reset();
		}

		/// <summary>
		/// 開始処理
		/// </summary>
		void Begin(void);

		/// <summary>
		/// 終了処理
		/// </summary>
		void End(void);

		/// <summary>
		/// グラフィックスパイプラインをセットする
		/// </summary>
		/// <param name="gpipeline"></param>
		void SetGraphicsPipeline(Pipeline& gpipeline);

		/// <summary>
		/// コンピュートパイプラインをセットする
		/// </summary>
		/// <param name="gpipeline"></param>
		void SetComputePipeline(Pipeline& cpipeline);

		/// <summary>
		/// プリミティブタイプをセットする
		/// </summary>
		/// <param name="type"></param>
		void SetPrimitiveType(PrimitiveType type);

		/// <summary>
		/// シザーレクトをセットする
		/// </summary>
		/// <param name="leftTop"> 左上 </param>
		/// <param name="rightBottom"> 右下 </param>
		void SetScissorrect(const glm::ivec2& leftTop, const glm::ivec2& rightBottom);

		/// <summary>
		/// ビューポートをセットする
		/// </summary>
		/// <param name="leftTop"> 左上 </param>
		/// <param name="size"> サイズ </param>
		/// <param name="depthMin"> 深度最小値 </param>
		/// <param name="depthMax"> 深度最大値 </param>
		void SetViewPort(const glm::vec2& leftTop, const glm::vec2& size, float depthMin = 0.0f, float depthMax = 1.0f);

		/// <summary>
		/// 頂点ビューをセットする
		/// </summary>
		/// <param name="view"></param>
		void SetVertexView(VertexView& view);

		/// <summary>
		/// インデックスビューをセットする
		/// </summary>
		/// <param name="view"></param>
		void SetIndexView(IndexView& view);

		// シェーダリソースセット系 //

		/// <summary>
		///  指定のパラメータインデックスに指定のビューのインデックスを先頭にセットする
		/// </summary>
		/// <param name="views"> シェーダリソースビュー </param>
		/// <param name="paramIdx"> パラメーターのインデックス </param>
		void SetShaderResourceView(ShaderResourceViews& views, std::uint64_t paramIdx);

		/// <summary>
		/// コンピュートシェーダーに指定のパラメータインデックスに指定のビューのインデックスを先頭にセットする
		/// </summary>
		/// <param name="views"></param>
		/// <param name="paramIdx"></param>
		void SetShaderResourceViewComputeShader(ShaderResourceViews& views, std::uint64_t paramIdx);

		/// <summary>
		/// 指定のパラメータインデックスに指定のサンプラービューのインデックスを先頭にセットする
		/// </summary>
		/// <param name="views"> サンプラービュー </param>
		/// <param name="paramIdx"> パラメーターのインデックス </param>
		void SetSamplerView(SamplerViews& views, std::uint64_t paramIdx);

		// 描画系 //

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="vertexCount"> 頂点数 </param>
		/// <param name="instanceCount"> インスタンス数 </param>
		void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1);

		/// <summary>
		/// インデックス付きで描画
		/// </summary>
		/// <param name="indexCount"> インデックス数 </param>
		/// <param name="instanceNum"> インスタンス数 </param>
		/// <param name="offset"> オフセット </param>
		void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum = 1, std::uint32_t offset = 0);

		/// <summary>
		/// コンピュートシェーダをディスパッチする
		/// </summary>
		/// <param name="count"> ディスパッチ数 </param>
		void Dispatch(const glm::u32vec3& count);

		/// <summary>
		/// レンダーターゲットを開始状態にする
		/// </summary>
		/// <param name="resource"></param>
		void TransitionRenderTargetBegin(ImageResource& resource);

		/// <summary>
		/// レンダーターゲットを終了状態にする(Commonとか)
		/// </summary>
		/// <param name="resource"></param>
		void TransitionRenderTargetEnd(ImageResource& resource);

		/// <summary>
		/// シェーダー使用状態にする
		/// </summary>
		/// <param name="resource"></param>
		void TransitionShaderResourceBegin(ImageResource& resource);

		/// <summary>
		/// シェーダー使用状態から終了する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionShaderResourceEnd(ImageResource& resource);

		/// <summary>
		/// デプス使用状態にする
		/// </summary>
		/// <param name="resource"></param>
		void TransitionDepthBegin(ImageResource& resource);

		/// <summary>
		/// デプス使用状態から終了する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionDepthEnd(ImageResource& resource);

		/// <summary>
		/// Unorderedなリソースとしての使用状態を開始する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionUnorderedAccessBegin(BufferResource& resource);

		/// <summary>
		/// Unorderedなリソースとしての使用状態を終了する
		/// </summary>
		/// <param name="resource"></param>
		void TransitionUnorderedAccessEnd(BufferResource& resource);

		/// <summary>
		/// バッファからテクスチャをコピーする
		/// </summary>
		/// <param name="dest"></param>
		/// <param name="src"></param>
		void CopyTexture(ImageResource& dest, BufferResource& src);

		/// <summary>
		/// MSAAのResolve処理を行う
		/// </summary>
		/// <param name="dest"></param>
		/// <param name="src"></param>
		void Resolve(ImageResource& dest, ImageResource& src);

		/// <summary>
		/// レンダーターゲットをセットする
		/// </summary>
		/// <param name="renderTargetViews"> レンダーターゲットのビュー </param>
		/// <param name="depthViews">　デプスステンシルのビュー　</param>
		/// <param name="rtClear"> レンダーターゲットのクリアカラー </param>
		/// <param name="rtRange"> ビューの範囲 </param>
		/// <param name="depthClear"> デプスステンシルのクリアカラー </param>
		/// <param name="depthIndex"> デプスステンシルのビューのインデックス </param>
		void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, std::optional<std::span<float, 4>> rtClear, std::pair<std::uint32_t, std::uint32_t> rtRange, std::optional<float> depthClear, std::uint32_t depthIndex);

		/// <summary>
		/// レンダーターゲットをセットする
		/// </summary>
		/// <param name="renderTargetViews"></param>
		/// <param name="rtClear"> レンダーターゲットのクリアカラー </param>
		/// <param name="rtRange"> ビューの範囲 </param>
		void SetRenderTarget(RenderTargetViews& renderTargetViews, std::optional<std::span<float, 4>> rtClear, std::pair<std::uint32_t, std::uint32_t> rtRange);

		/// <summary>
		/// バッファリソースをコピーする
		/// </summary>
		/// <param name="dest"></param>
		/// <param name="src"></param>
		void CopyBuffer(BufferResource& dest, BufferResource& src);

#ifdef USE_IMGUI
		/// <summary>
		/// ImGui用のコマンドをセットする
		/// </summary>
		/// <param name="data"></param>
		void SetImguiCommand(ImDrawData* data) const;
#endif
	private:
		CommandList(void* dummy);
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



