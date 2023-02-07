#pragma once



namespace Eugene
{
	class Sampler;
	class SamplerViews
	{
	public:
		virtual ~SamplerViews();
		virtual void CreateSampler(Sampler& sampler, size_t idx) = 0;
		virtual void* GetViews(void) const = 0;
	protected:
		SamplerViews();
	};
}
