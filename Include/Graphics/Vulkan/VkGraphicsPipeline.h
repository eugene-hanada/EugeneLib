#pragma once
#include "../GraphicsCommon.h"

#include <vulkan/vulkan.hpp>
#include "../../../Include/Utils//ArgsSpan.h"

namespace Eugene
{
	class ResourceBindLayout;
	class Pipeline
	{
	public:

		struct Data
		{
			std::shared_ptr<vk::UniquePipelineLayout> layout_;
			vk::UniquePipeline pipeline_;
		};
		using PipelineType = Data;

		PipelineType& GetPipeline(void) noexcept
		{
			return data_;
		}

		void Final() noexcept
		{
			data_.pipeline_.reset();
			data_.layout_.reset();
		}


		Pipeline(Pipeline&& pipeline) noexcept 
		{
			data_.layout_ = std::move(pipeline.data_.layout_);
			data_.pipeline_ = std::move(pipeline.data_.pipeline_);
		}
		Pipeline& operator=(Pipeline&& pipeline) noexcept
		{
			data_.layout_ = std::move(pipeline.data_.layout_);
			data_.pipeline_ = std::move(pipeline.data_.pipeline_);
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

		// GraphicsPipeline を介して継承されました
		PipelineType data_;

		friend class Graphics;
	};
}