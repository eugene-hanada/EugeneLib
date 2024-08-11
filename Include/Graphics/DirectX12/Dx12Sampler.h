#pragma once
#include "../GraphicsCommon.h"
#include <d3d12.h>


namespace Eugene
{
	class Sampler
	{
	public:
		Sampler()noexcept
		{
		}

		void Final()noexcept
		{

		}

		Sampler(Sampler&& sampler) noexcept :
			desc_{sampler.desc_}
		{
		}


		Sampler& operator=(Sampler&& sampler) noexcept
		{
			desc_ = sampler.desc_;
		}

	private:
		Sampler(const SamplerLayout& layout);

		Sampler(const Sampler& ) = delete;
		Sampler& operator=(const Sampler&) = delete;

		D3D12_SAMPLER_DESC desc_;
		friend class Graphics;
		friend class SamplerViews;
	};
}
