#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Eugene
{
	class Sampler;

	/// <summary>
	/// サンプラービュー
	/// </summary>
	class SamplerViews
	{
	public:
		SamplerViews() noexcept : size_{ 0 }
		{
		}

		/// <summary>
		/// ビューを生成する
		/// </summary>
		/// <param name="sampler"> サンプラー </param>
		/// <param name="idx"> インデックス </param>
		void CreateSampler(Sampler& sampler, std::uint32_t idx);

		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <returns> ビューのポインタ </returns>
		void* GetViews(void) noexcept
		{
			return descriptorHeap_.Get();
		}

		/// <summary>
		/// サイズを取得(保持できるビューの数)
		/// </summary>
		/// <returns></returns>
		std::uint32_t GetSize() const noexcept
		{
			return size_;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			descriptorHeap_.Reset();
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="samplerViews"></param>
		SamplerViews(SamplerViews&& samplerViews) noexcept :
			descriptorHeap_{std::move(samplerViews.descriptorHeap_)},size_{samplerViews.size_}
		{
			size_ = 0;
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="samplerViews"></param>
		/// <returns></returns>
		SamplerViews& operator=(SamplerViews&& samplerViews) noexcept
		{
			descriptorHeap_ = std::move(samplerViews.descriptorHeap_);
			size_ = samplerViews.size_;
			samplerViews.size_ = 0;
		}
	private:
		SamplerViews(std::uint32_t size)
		{
			Init(size);
		}

		void Init(std::uint32_t size);

		SamplerViews(const SamplerViews& ) = delete;
		SamplerViews& operator=(const SamplerViews&) = delete;

		std::uint32_t size_;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		friend class Graphics;
	};
}
