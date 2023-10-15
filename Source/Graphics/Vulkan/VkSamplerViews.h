#pragma once
#include "../../../Include/Graphics/SamplerViews.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Common/ArgsSpan.h"
#include "../../../Include/Graphics/ResourceBindLayout.h"

namespace Eugene
{
	class VkSamplerViews :
		public SamplerViews
	{
	public:
		VkSamplerViews(const vk::Device& device, const ArgsSpan<Bind>& viewTypes);
	private:
		// SamplerViews ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		void CreateSampler(Sampler& sampler, std::uint64_t idx) final;
		void* GetViews(void) const final;
		vk::UniqueDescriptorSet descriptorSet_;
		vk::UniqueDescriptorPool descriptorPool_;
		vk::UniqueDescriptorSetLayout layout_;
		std::vector<std::pair<std::uint32_t, std::uint32_t>> typeData_;
	};
}
