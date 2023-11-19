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
		~VkResourceBindLayout();
	private:

		/// <summary>
		/// ディスクリプタセットのレイアウト
		/// </summary>
		std::vector<vk::DescriptorSetLayout> descriptorLayoutArray_;

		/// <summary>
		/// パイプラインのレイアウト
		/// </summary>
		vk::UniquePipelineLayout pipelineLayout_;

		friend class VkGraphicsPipeline;
	};
}
