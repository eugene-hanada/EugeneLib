#pragma once
#include <cstdint>


namespace Eugene
{
	class Sampler;

	/// <summary>
	/// �T���v���[�p�r���[
	/// </summary>
	class SamplerViews
	{
	public:
		virtual ~SamplerViews();

		/// <summary>
		/// �T���v���[�r���[����
		/// </summary>
		/// <param name="sampler"> �T���v���[ </param>
		/// <param name="idx"> �C���f�b�N�X </param>
		virtual void CreateSampler(Sampler& sampler, std::uint64_t idx) = 0;
		virtual void* GetViews(void) const = 0;
	protected:
		SamplerViews();
	};
}
