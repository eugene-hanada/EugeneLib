#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

namespace Eugene
{
	class ImageResource;
	class DepthStencilViews
	{
	public:
		DepthStencilViews() :
			size_{0}
		{
		}

		void Create(ImageResource& resource, std::uint32_t idx);
		void* GetViews(void)
		{
			return descriptorHeap_.Get();
		}

		DepthStencilViews(const DepthStencilViews& views);
		DepthStencilViews& operator=(const DepthStencilViews& views);

		std::uint32_t GetSize() const
		{
			return size_;
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
