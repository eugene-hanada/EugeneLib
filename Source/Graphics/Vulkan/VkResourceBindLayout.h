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
		/// �f�B�X�N���v�^�Z�b�g�̃��C�A�E�g
		/// </summary>
		std::vector<vk::DescriptorSetLayout> descriptorLayoutArray_;

		/// <summary>
		/// �p�C�v���C���̃��C�A�E�g
		/// </summary>
		vk::UniquePipelineLayout pipelineLayout_;

		friend class VkGraphicsPipeline;
	};
}
