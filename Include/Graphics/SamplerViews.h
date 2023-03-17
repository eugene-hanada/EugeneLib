#pragma once
#include <cstdint>


namespace Eugene
{
	class Sampler;

	/// <summary>
	/// サンプラー用ビュー
	/// </summary>
	class SamplerViews
	{
	public:
		virtual ~SamplerViews();

		/// <summary>
		/// サンプラービュー生成
		/// </summary>
		/// <param name="sampler"> サンプラー </param>
		/// <param name="idx"> インデックス </param>
		virtual void CreateSampler(Sampler& sampler, std::uint64_t idx) = 0;
		virtual void* GetViews(void) const = 0;
	protected:
		SamplerViews();
	};
}
