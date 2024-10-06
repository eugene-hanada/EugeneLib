#pragma once
#include "../GraphicsCommon.h"
#include <d3d12.h>


namespace Eugene
{
	/// <summary>
	/// サンプラー
	/// </summary>
	class Sampler
	{
	public:
		Sampler()noexcept
		{
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		constexpr void Final()noexcept
		{
			//何もしない
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="sampler"></param>
		Sampler(Sampler&& sampler) noexcept :
			desc_{sampler.desc_}
		{
		}

		/// <summary>
		/// ムーブ演算子
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
