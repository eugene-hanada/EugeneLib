#pragma once
#include "../GraphicsCommon.h"
#include <wrl.h>
#include <d3d12.h>
#include "../../../Include/Utils/ArgsSpan.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;

namespace Eugene
{
	class ResourceBindLayout;

	/// <summary>
	/// パイプラインクラス
	/// </summary>
	class Pipeline
	{
	public:
		Pipeline() noexcept = default;


		struct PipeLineSet
		{
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
			Microsoft::WRL::ComPtr<ID3D12PipelineState> state_;
		};

		/// <summary>
		/// API側のパイプラインを取得する
		/// </summary>
		/// <returns> パイプラインのポインタ </returns>
		void* GetPipeline(void) noexcept
		{
			return &pipeline_;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			pipeline_.state_.Reset();
			pipeline_.rootSignature_.Reset();
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="pipeline"></param>
		Pipeline(Pipeline&& pipeline) noexcept
		{
			pipeline_.state_ = std::move(pipeline.pipeline_.state_);
			pipeline_.rootSignature_ = std::move(pipeline.pipeline_.rootSignature_);
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="pipeline"></param>
		/// <returns></returns>
		Pipeline& operator=(Pipeline&& pipeline) noexcept
		{
			pipeline_.state_ = std::move(pipeline.pipeline_.state_);
			pipeline_.rootSignature_ = std::move(pipeline.pipeline_.rootSignature_);
			return *this;
		}

		Pipeline(const Pipeline& pipeline) = delete;
		Pipeline& operator=(const Pipeline& pipeline) = delete;
	private:

		/// <summary>
		/// グラフィックス版コンストラクタ
		/// </summary>
		/// <param name="resourceBindLayout"></param>
		/// <param name="layout"></param>
		/// <param name="shaders"></param>
		/// <param name="rendertarges"></param>
		/// <param name="topologyType"></param>
		/// <param name="isCulling"></param>
		/// <param name="useDepth"></param>
		/// <param name="sampleCount"></param>
		Pipeline(
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType,
			bool isCulling,
			bool useDepth,
			std::uint8_t sampleCount
		);

		/// <summary>
		/// コンピュート版コンストラクタ
		/// </summary>
		/// <param name="resourceBindLayout"></param>
		/// <param name="csShader"></param>
		Pipeline(
			ResourceBindLayout& resourceBindLayout,
			const std::span<const std::uint8_t> csShader
		);

		PipeLineSet pipeline_;

		friend class Graphics;
	};
}
