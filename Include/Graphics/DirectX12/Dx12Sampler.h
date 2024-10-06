#pragma once
#include "../GraphicsCommon.h"
#include <d3d12.h>


namespace Eugene
{
	/// <summary>
	/// �T���v���[
	/// </summary>
	class Sampler
	{
	public:
		Sampler()noexcept
		{
		}

		/// <summary>
		/// �I������
		/// </summary>
		constexpr void Final()noexcept
		{
			//�������Ȃ�
		}

		/// <summary>
		/// ���[�u�R���X�g���N�^
		/// </summary>
		/// <param name="sampler"></param>
		Sampler(Sampler&& sampler) noexcept :
			desc_{sampler.desc_}
		{
		}

		/// <summary>
		/// ���[�u���Z�q
		/// </summary>
		/// <param name="sampler"></param>
		/// <returns></returns>
		Sampler& operator=(Sampler&& sampler) noexcept
		{
			desc_ = sampler.desc_;
			return *this;
		}
		Sampler(const Sampler&) = delete;
		Sampler& operator=(const Sampler&) = delete;
	private:
		Sampler(const SamplerLayout& layout);

		D3D12_SAMPLER_DESC desc_;
		friend class Graphics;
		friend class SamplerViews;
	};
}
