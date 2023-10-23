#pragma once
#include "../../../Include/Graphics/BufferResource.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	/// <summary>
	/// �o�b�t�@�ɕK�v�ȃf�[�^
	/// </summary>
	struct VkBufferData
	{
		/// <summary>
		/// �f�o�C�X������
		/// </summary>
		vk::UniqueDeviceMemory memory_;

		/// <summary>
		/// �o�b�t�@
		/// </summary>
		vk::UniqueBuffer buffer_;
	};

	class VkGraphics;
	class Image;

	/// <summary>
	/// �o�b�t�@�p(�萔�A���_�A�C���f�b�N�X�ȂǗp)
	/// </summary>
	class VkBufferResource :
		public BufferResource
	{
	public:
		VkBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size);
	private:

		// BufferResource ����Čp������܂���
		bool CanMap(void) const final;
		void* GetResource(void) final;
		std::uint64_t GetSize(void) final;

		/// <summary>
		/// �f�[�^
		/// </summary>
		VkBufferData data_;
	};

	/// <summary>
	/// �A�b�v���[�h�p
	/// </summary>
	class VkUploadableBufferResource :
		public BufferResource
	{
	public:
		VkUploadableBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size);
		VkUploadableBufferResource(const vk::Device& device, const VkGraphics& graphics, Image& image);
	private:
		// BufferResource ����Čp������܂���
		bool CanMap(void) const final;
		void* GetResource(void) final;
		std::uint64_t GetSize(void) final;
		void* Map(void) final;
		virtual void UnMap(void) final;

		/// <summary>
		/// �f�[�^
		/// </summary>
		VkBufferData data_;
	};
}

