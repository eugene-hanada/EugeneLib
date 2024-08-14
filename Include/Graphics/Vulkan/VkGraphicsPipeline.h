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
			vk::PipelineLayout layout_;
			vk::UniquePipeline pipeline_;
		};
		using PipelineType = Data;

		PipelineType& GetPipeline(void) noexcept
		{
			return data_;
		}

		void Final() noexcept
		{
			data_.layout_ = vk::PipelineLayout{};
			data_.pipeline_.release();
		}

		void Init(ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType,
			bool isCulling,
			bool useDepth,
			std::uint8_t sampleCount);

		void Init(ResourceBindLayout& resourceBindLayout,
			const Shader& csShader);

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
		)
		{
			Init(resourceBindLayout, layout, shaders,rendertarges, topologyType, isCulling, useDepth, sampleCount);
		}

		Pipeline(
			ResourceBindLayout& resourceBindLayout,
			const Shader& csShader
		)
		{
			Init(resourceBindLayout, csShader);
		}

		// GraphicsPipeline を介して継承されました
		Data data_;

		friend class Graphics;
	};
}