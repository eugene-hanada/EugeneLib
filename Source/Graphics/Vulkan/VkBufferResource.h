#pragma once
#include "../../../Include/Graphics/BufferResource.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/ThirdParty/VulkanMemoryAllocator-Hpp/include/vk_mem_alloc.hpp"

namespace Eugene
{
	/// <summary>
	/// バッファに必要なデータ
	/// </summary>
	struct VkBufferData
	{

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
		std::uint64_t size_{0ull};
	};

	class VkGraphics;
	class Image;

	/// <summary>
	/// バッファ用(定数、頂点、インデックスなど用)
	/// </summary>
	class VkBufferResource :
		public BufferResource
	{
	public:
		VkBufferResource(const vma::Allocator& allocator, std::uint64_t size, bool isUnordered);
	private:

		// BufferResource を介して継承されました
		bool CanMap(void) const final;
		void* GetResource(void) final;
		std::uint64_t GetSize(void) final;

		/// <summary>
		/// データ
		/// </summary>
		VkBufferData data_;
	};

	/// <summary>
	/// アップロード用
	/// </summary>
	class VkUnloadableBufferResource :
		public BufferResource
	{
	public:
		VkUnloadableBufferResource(const vma::Allocator& allocator, std::uint64_t size, bool isUnordered);
		VkUnloadableBufferResource(const vma::Allocator& allocator, Image& image);
		~VkUnloadableBufferResource();
	private:
		// BufferResource を介して継承されました
		bool CanMap(void) const final;
		void* GetResource(void) final;
		std::uint64_t GetSize(void) final;
		void* Map(void) final;
		virtual void UnMap(void) final;

		/// <summary>
		/// データ
		/// </summary>
		VkBufferData data_;

		/// <summary>
		/// Vulkanのメモリアロケーター
		/// </summary>
		const vma::Allocator& allocator_;

		/// <summary>
		/// マップした回数
		/// </summary>
		std::uint64_t mapCount_;
	};
}

