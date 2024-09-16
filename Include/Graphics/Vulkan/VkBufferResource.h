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
		BufferResource() noexcept :
			size_{0},canMap_{false}, mapCount_{0}
		{
		}
		
		~BufferResource() noexcept
		{
			AllUnMap();
		}

		/// <summary>
		/// マップ可能か？
		/// </summary>
		/// <returns> マップ可能な場合true不可能な場合false </returns>
		bool CanMap(void) const noexcept
		{
			return canMap_;
		}

		/// <summary>
		/// API側のリソースを取得する
		/// </summary>
		/// <returns> リソースのポインタ </returns>
		void* GetResource(void) noexcept
		{
			return &*buffer_;
		}

		/// <summary>
		/// サイズを取得(バイト数)
		/// </summary>
		/// <returns></returns>
		std::uint64_t GetSize(void) const noexcept
		{
			return size_;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final()noexcept
		{
			AllUnMap();
			buffer_.reset();
			allocation_.reset();
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="bufferResource"></param>
		BufferResource(BufferResource&& bufferResource) noexcept :
			buffer_{ std::move(bufferResource.buffer_) }, allocation_{ std::move(bufferResource.allocation_) }, canMap_{ bufferResource.canMap_ },
			mapCount_{bufferResource.mapCount_},size_{bufferResource.size_}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="bufferResource"></param>
		/// <returns></returns>
		BufferResource& operator=(BufferResource&& bufferResource) noexcept
		{
			buffer_ = std::move(bufferResource.buffer_);
			allocation_ = std::move(bufferResource.allocation_);
			canMap_ = bufferResource.canMap_;
			mapCount_ = bufferResource.mapCount_;
			size_ = bufferResource.size_;
		}

		/// <summary>
		/// マップする
		/// </summary>
		/// <returns></returns>
		void* Map() noexcept;

		/// <summary>
		/// アンマップする
		/// </summary>
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

