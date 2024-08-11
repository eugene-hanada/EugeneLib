#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Eugene
{
	class Sampler;
	class SamplerViews
	{
	public:
		SamplerViews() noexcept : size_{ 0 }
		{
		}

		
	
		void CreateSampler(Sampler& sampler, std::uint32_t idx);
		void* GetViews(void) noexcept
		{
			return descriptorHeap_.Get();
		}

		std::uint32_t GetSize() const noexcept
		{
			return size_;
		}

		void Final() noexcept
		{
			descriptorHeap_.Reset();
		}

		SamplerViews(SamplerViews&& samplerViews) noexcept :
			descriptorHeap_{std::move(samplerViews.descriptorHeap_)},size_{samplerViews.size_}
		{
			size_ = 0;
		}
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
