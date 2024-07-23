#pragma once
#include "../../../Include/Graphics/GraphicsPipeline.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Utils//ArgsSpan.h"

namespace Eugene
{
	class ResourceBindLayout;
	class VkGraphicsPipeline :
		public Pipeline
	{
	public:
		VkGraphicsPipeline(const vk::Device& device,
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType,
			bool isCulling,
			bool useDepth,
			std::uint8_t sampleCount
		);

		struct Data
		{
			vk::PipelineLayout layout_;
			vk::UniquePipeline pipeline_;
		};
		using PipelineType = Data;
	private:

		// GraphicsPipeline を介して継承されました
		void* GetPipeline(void) final;
		Data data_;
	};
}