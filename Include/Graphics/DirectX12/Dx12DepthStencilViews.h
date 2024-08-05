#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Eugene
{
	class ImageResource;
	class DepthStencilViews
	{
	public:
		DepthStencilViews() noexcept :
			size_{0}, isShaderVisible_{false}
		{
		}

		void Create(ImageResource& resource, std::uint32_t idx);
		void* GetViews(void)
		{
			return descriptorHeap_.Get();
		}

		DepthStencilViews(const DepthStencilViews& views);
		DepthStencilViews& operator=(const DepthStencilViews& views);


		DepthStencilViews(DepthStencilViews&& views) noexcept :
			descriptorHeap_{ std::move(views.descriptorHeap_) }, size_{ views.size_ }, isShaderVisible_{views.isShaderVisible_}
		{
		}

		DepthStencilViews& operator=(DepthStencilViews&& views) noexcept
		{
			descriptorHeap_ = std::move(views.descriptorHeap_);
			size_ = views.size_;
			isShaderVisible_ = views.isShaderVisible_;

			views.size_ = 0;
		}

		std::uint32_t GetSize() const
		{
			return size_;
		}

		void Final() noexcept
		{
			descriptorHeap_.Reset();
			size_ = 0;
		}

	private:
		void Init(std::uint32_t size, bool isShaderVisible);

		DepthStencilViews(std::uint32_t size, bool isShaderVisible)
		{
			Init(size, isShaderVisible);
		}

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		std::uint32_t size_;

		bool isShaderVisible_;

		friend class Graphics;
	};
}
