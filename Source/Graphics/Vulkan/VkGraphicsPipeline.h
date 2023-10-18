#pragma once
#include "../../../Include/Graphics/GraphicsPipeline.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Common/ArgsSpan.h"

namespace Eugene
{
	class ResourceBindLayout;
	class VkGraphicsPipeline :
		public GraphicsPipeline
	{
	public:
		VkGraphicsPipeline(const vk::Device& device,
			ResourceBindLayout& resourceBindLayout,
			const ArgsSpan<ShaderInputLayout>& layout,
			const ArgsSpan<ShaderPair>& shaders,
			const ArgsSpan<RendertargetLayout>& rendertarges,
			TopologyType topologyType,
			bool isCulling,
			bool useDepth);

		struct Data
		{
			vk::PipelineLayout layout_;
			vk::UniquePipeline pipeline_;
		};
		using PipelineType = Data;
	private:

		// GraphicsPipeline ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		void* GetPipeline(void) final;
		Data data_;
	};
}