#pragma once
#include "../../../Include/Graphics/Sampler.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	class VkSampler :
		public Sampler
	{
	public:
		VkSampler(const vk::Device& device, const SamplerLayout& layout);
	private:

		/// <summary>
		/// ƒTƒ“ƒvƒ‰[
		/// </summary>
		vk::UniqueSampler sampler_;
		friend class VkSamplerViews;
	};
}