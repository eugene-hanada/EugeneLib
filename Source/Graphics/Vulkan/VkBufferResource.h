#pragma once
#include "../../../Include/Graphics/BufferResource.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	/// <summary>
	/// バッファに必要なデータ
	/// </summary>
	struct VkBufferData
	{
		/// <summary>
		/// デバイスメモリ
		/// </summary>
		vk::UniqueDeviceMemory memory_;

		/// <summary>
		/// バッファ
		/// </summary>
		vk::UniqueBuffer buffer_;
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
		VkBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size);
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
	class VkUploadableBufferResource :
		public BufferResource
	{
	public:
		VkUploadableBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size);
		VkUploadableBufferResource(const vk::Device& device, const VkGraphics& graphics, Image& image);
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
	};
}

