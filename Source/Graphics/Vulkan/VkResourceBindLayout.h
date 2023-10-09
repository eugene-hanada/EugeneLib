#pragma once
#include "../../../Include/Graphics/ResourceBindLayout.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include "../../../Include/Common/ArgsSpan.h"

namespace Eugene
{
	class VkResourceBindLayout :
		public ResourceBindLayout
	{
	public:
		VkResourceBindLayout(const vk::Device& device, const ArgsSpan<ArgsSpan<Bind>>& viewTypes);
	private:
		std::vector<vk::DescriptorSetLayout> descriptorLayoutArray_;
		vk::PipelineLayout pipelineLayout_;

		friend class VkGraphicsPipeline;
	};
}
