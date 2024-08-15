#pragma once
#include "../GraphicsCommon.h"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>


namespace Eugene
{
	class Image;

	/// <summary>
	/// バッファ用(定数、頂点、インデックスなど用)
	/// </summary>
	class BufferResource
	{
	public:
		using ResrouceType = vk::Buffer;
		BufferResource() noexcept :
			size_{0},canMap_{false}, mapCount_{0}
		{
		}
		
		~BufferResource() noexcept
		{
			AllUnMap();
		}

		bool CanMap(void) const noexcept
		{
			return canMap_;
		}

		ResrouceType& GetResource(void) noexcept
		{
			return *buffer_;
		}

		std::uint64_t GetSize(void) const noexcept
		{
			return size_;
		}

		void Final()noexcept
		{
			AllUnMap();
			buffer_.reset();
			allocation_.reset();
		}

		BufferResource(BufferResource&& bufferResource) noexcept :
			buffer_{ std::move(bufferResource.buffer_) }, allocation_{ std::move(bufferResource.allocation_) }, canMap_{ bufferResource.canMap_ },
			mapCount_{bufferResource.mapCount_},size_{bufferResource.size_}
		{
		}

		BufferResource& operator=(BufferResource&& bufferResource) noexcept
		{
			buffer_ = std::move(bufferResource.buffer_);
			allocation_ = std::move(bufferResource.allocation_);
			canMap_ = bufferResource.canMap_;
			mapCount_ = bufferResource.mapCount_;
			size_ = bufferResource.size_;
		}

		void* Map() noexcept;
		void UnMap() noexcept;
		BufferResource(const BufferResource&) = delete;
		BufferResource& operator=(const BufferResource&) = delete;
	private:
		BufferResource(Image& image);
		BufferResource(std::uint64_t size, bool isUnordered, GpuResourceType type);

		
		void AllUnMap();

		/// <summary>
		/// アロケーターで確保したメモリ
		/// </summary>
		vma::UniqueAllocation allocation_;


		/// <summary>
		/// バッファ
		/// </summary>
		vma::UniqueBuffer buffer_;

		/// <summary>
		/// バッファのサイズ(バイト数)
		/// </summary>
		std::uint64_t size_;

		std::uint64_t mapCount_;


		bool canMap_;

		friend class CommandList;
		friend class Graphics;
	};


}

