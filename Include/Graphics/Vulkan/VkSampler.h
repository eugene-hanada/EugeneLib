#pragma once
#include "../GraphicsCommon.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	/// <summary>
	/// サンプラー
	/// </summary>
	class Sampler
	{
	public:
		Sampler() = default;

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final()noexcept
		{
			sampler_.reset();
		}

		Sampler(Sampler&& sampler) noexcept :
			sampler_{ std::move(sampler.sampler_) }
		{
		}
		Sampler& operator=(Sampler&& sampler) noexcept
		{
			sampler_ = std::move(sampler.sampler_);
			return *this;
		}
		Sampler(const Sampler&) = delete;
		Sampler& operator=(const Sampler&) = delete;
	private:


		Sampler(const SamplerLayout& layout);

		/// <summary>
		/// サンプラー
		/// </summary>
		vk::UniqueSampler sampler_;

		friend class Graphics;
		friend class SamplerViews;
	};
}