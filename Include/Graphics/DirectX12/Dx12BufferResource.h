#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "../../ThirdParty/D3D12MemoryAllocator/include/D3D12MemAlloc.h"
#include "../../../ThirdParty/glm/glm/vec2.hpp"
#include "../GraphicsCommon.h"


namespace D3D12MA
{
	struct Allocation;
	struct Allocator;
}


namespace Eugene
{
	class Image;

	class BufferResource
	{
	public:
		BufferResource() noexcept = default;
		BufferResource(std::uint64_t size, bool isUnordered, GpuResourceType type);
		BufferResource(Image& image);
		~BufferResource() = default;

		bool CanMap(void) const noexcept
		{
			return canMap_;
		}
		
		void* GetResource(void) const
		{
			return resource_.Get();
		}

		std::uint64_t GetSize(void) const
		{
			return resource_->GetDesc().Width;
		}

		void Final()noexcept
		{
			resource_.Reset();
			allocation_.Reset();
		}

		BufferResource(BufferResource&& bufferResource) noexcept :
			resource_{std::move(bufferResource.resource_)}, allocation_{std::move(bufferResource.allocation_)}, canMap_{bufferResource.canMap_}
		{
		}

		BufferResource& operator=(BufferResource&& bufferResource) noexcept
		{
			resource_ = std::move(bufferResource.resource_);
			allocation_ = std::move(bufferResource.allocation_);
			canMap_ = bufferResource.canMap_;
		}

		void* Map() noexcept
		{
			void* ptr{ nullptr };
			resource_->Map(0, nullptr, &ptr);
			return ptr;
		}

		void UnMap() noexcept
		{
			resource_->Unmap(0, nullptr);
		}
	private:
		BufferResource(const BufferResource&) = delete;
		BufferResource& operator=(const BufferResource&) = delete;

		/// <summary>
		/// アロケーターで確保したメモリ
		/// </summary>
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation_;

		/// <summary>
		/// リソース
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

		bool canMap_;

	};
}
