#pragma once
#include "../GraphicsCommon.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	class Sampler
	{
	public:
		Sampler() = default;

		void Final()noexcept
		{
			sampler_.release();
		}
		Sampler(Sampler&& sampler) noexcept :
			sampler_{ std::move(sampler.sampler_) }
		{
		}


	private:
		Sampler(const Sampler&) = delete;
		Sampler& operator=(const Sampler&) = delete;

		Sampler(const SamplerLayout& layout);

		/// <summary>
		/// サンプラー
		/// </summary>
		vk::UniqueSampler sampler_;

		friend class Graphics;
		friend class SamplerViews;
	};
}