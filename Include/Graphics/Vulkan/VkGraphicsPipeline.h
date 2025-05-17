#pragma once
#include "../GraphicsCommon.h"

#include <vulkan/vulkan.hpp>
#include "../../../Include/Utils//ArgsSpan.h"

namespace Eugene
{
	class ResourceBindLayout;

	/// <summary>
	/// パイプラインクラス
	/// </summary>
	class Pipeline
	{
	public:

		struct Data
		{
			std::shared_ptr<vk::UniquePipelineLayout> layout_;
			vk::UniquePipeline pipeline_;
			std::uint32_t pushConstantSize_{ 0 };
		};

		/// <summary>
		/// API側のパイプラインを取得する
		/// </summary>
		/// <returns> パイプラインのポインタ </returns>
		void* GetPipeline(void) noexcept
		{
			return &data_;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			data_.pipeline_.reset();
			data_.layout_.reset();
			data_.pushConstantSize_ = 0;
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="pipeline"></param>
		Pipeline(Pipeline&& pipeline) noexcept 
		{
			data_.layout_ = std::move(pipeline.data_.layout_);
			data_.pipeline_ = std::move(pipeline.data_.pipeline_);
			data_.pushConstantSize_ = pipeline.data_.pushConstantSize_;
			pipeline.data_.pushConstantSize_ = 0;
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="pipeline"></param>
		/// <returns></returns>
		Pipeline& operator=(Pipeline&& pipeline) noexcept
		{
			data_.layout_ = std::move(pipeline.data_.layout_);
			data_.pipeline_ = std::move(pipeline.data_.pipeline_);
			data_.pushConstantSize_ = pipeline.data_.pushConstantSize_;
			return *this;
		}

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline() = default;
	private:

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

		Pipeline(
			ResourceBindLayout& resourceBindLayout,
			const Shader& csShader
		);

		Data data_;



		friend class Graphics;
		friend class CommandList;
	};
}