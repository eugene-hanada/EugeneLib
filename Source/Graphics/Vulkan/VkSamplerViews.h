#pragma once
#include "../../../Include/Graphics/SamplerViews.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	class VkSamplerViews :
		public SamplerViews
	{
	public:
		VkSamplerViews(const vk::Device& device, std::uint64_t size);
	private:
		// SamplerViews を介して継承されました
		void CreateSampler(Sampler& sampler, std::uint64_t idx) final;
		void* GetViews(void) const final;
	};
}
