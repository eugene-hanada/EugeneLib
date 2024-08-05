#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Eugene
{
	class ImageResource;

	class RenderTargetViews
	{
	public:
		RenderTargetViews() noexcept :
			size_{0}, isShaderVisible_{false}
		{
		}

		void Create(ImageResource& resource, std::uint32_t idx);

		void* GetViews(void)
		{
			return descriptorHeap_.Get();
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

		RenderTargetViews(RenderTargetViews&& views) noexcept :
			descriptorHeap_{std::move(views.descriptorHeap_)}, size_{std::move(views.size_)}, isShaderVisible_{views.isShaderVisible_}
		{
			views.size_ = 0;
		}

		RenderTargetViews& operator=(RenderTargetViews&& views) noexcept
		{
			descriptorHeap_ = std::move(views.descriptorHeap_);
			size_ = views.size_;
			views.size_ = 0;
			isShaderVisible_ = views.isShaderVisible_;
		}

		RenderTargetViews(const RenderTargetViews& views) :
			isShaderVisible_{ views.isShaderVisible_ }
		{
			*this = views;
		}

		RenderTargetViews& operator=(const RenderTargetViews& views);
	private:

		void Init(std::uint32_t size, bool isShaderVisible);

		RenderTargetViews(std::uint32_t size, bool isShaderVisible)
		{
			Init(size, isShaderVisible);
		}

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		std::uint32_t size_;

		bool isShaderVisible_;

		friend class Graphics;
	};
}
