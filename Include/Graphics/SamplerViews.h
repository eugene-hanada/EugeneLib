#pragma once
#include <cstdint>


namespace Eugene
{
	class Sampler;
	class SamplerViews
	{
	public:
		virtual ~SamplerViews();
		virtual void CreateSampler(Sampler& sampler, std::uint64_t idx) = 0;
		virtual void* GetViews(void) const = 0;
	protected:
		SamplerViews();
	};
}
