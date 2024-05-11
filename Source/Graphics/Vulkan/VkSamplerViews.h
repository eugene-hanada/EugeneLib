#pragma once
#include "../../../Include/Graphics/SamplerViews.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Utils//ArgsSpan.h"
#include "../../../Include/Graphics/ResourceBindLayout.h"

namespace Eugene
{
	class VkSamplerViews :
		public SamplerViews
	{
	public:
		VkSamplerViews(const vk::Device& device, const ArgsSpan<Bind>& viewTypes);
		struct Data
		{
			vk::UniqueDescriptorSet descriptorSet_;
			vk::UniqueDescriptorSetLayout layout_;
		};
	private:
		// SamplerViews を介して継承されました
		void CreateSampler(Sampler& sampler, std::uint64_t idx) final;
		void* GetViews(void) final;
		vk::UniqueDescriptorPool descriptorPool_;
		Data data_;
		std::vector<std::pair<std::uint32_t, std::uint32_t>> typeData_;
	};
}
