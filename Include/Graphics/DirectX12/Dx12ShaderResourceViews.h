#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Eugene
{
	class ImageResource;
	class BufferResource;
	class  ShaderResourceViews
	{
	public:
		ShaderResourceViews() noexcept :
			size_{0}
		{
		}

		void CreateTexture(ImageResource& resource, std::uint32_t idx);
		void CreateConstantBuffer(BufferResource& resource, std::uint32_t idx) ;
		void CreateCubeMap(ImageResource& resource, std::uint32_t idx) ;
		void CreateUnorderedAccessBuffer(BufferResource& resource, std::uint32_t idx, std::uint32_t numElements, std::uint64_t strideSize);

		void* GetViews(void)
		{
			return descriptorHeap_.Get();
		}
		std::uint64_t GetImg(void);

		std::uint32_t GetSize() const noexcept
		{
			return size_;
		}
		void Final()noexcept
		{
			size_ = 0;
			descriptorHeap_.Reset();
		}


		ShaderResourceViews(ShaderResourceViews&& views) noexcept :
			descriptorHeap_{std::move(views.descriptorHeap_)}, size_{views.size_}
		{
			views.size_ = 0;
		}

		ShaderResourceViews& operator=(ShaderResourceViews&& views) noexcept
		{
			descriptorHeap_ = std::move(views.descriptorHeap_);
			size_ = views.size_;
			views.size_ = 0;
		}

		ShaderResourceViews(const ShaderResourceViews& views)
		{
			*this = views;
		}

		ShaderResourceViews& operator=(const ShaderResourceViews& views);
	protected:
		ShaderResourceViews(std::uint32_t size)
		{
			Init(size);
		}
		void Init(std::uint32_t size);
		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		std::uint32_t size_;

		friend class Graphics;
	};
}
