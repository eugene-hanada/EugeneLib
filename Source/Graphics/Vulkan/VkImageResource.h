#pragma once
#include <vulkan/vulkan.hpp>
#include "../../../Include/Graphics/ImageResource.h"

namespace Eugene
{
	class VkGraphics;
	class VkImageResource :
		public ImageResource
	{
	public:
		/// <summary>
		/// �e�N�X�`���p�̃R���X�g���N�^
		/// </summary>
		/// <param name="graphics"> �O���t�B�b�N�X�N���X </param>
		/// <param name="device"> Vulkan�̃f�o�C�X </param>
		/// <param name="info"> �e�N�X�`���̏�� </param>
		VkImageResource(const VkGraphics& graphics,const vk::Device& device, const TextureInfo& info);

		/// <summary>
		/// �f�u�X�o�b�t�@�̏ꍇ�̃R���X�g���N�^
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		/// <param name="clearValue"></param>
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const Vector2I& size, float clearValue);


		/// <summary>
		/// �����_�[�^�[�Q�b�g�̏ꍇ�̃R���X�g���N�^
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		/// <param name="format"></param>
		/// <param name="clearColor"></param>
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const Vector2I& size, Format format);

		/// <summary>
		/// �o�b�N�o�b�t�@�̏ꍇ�̃R���X�g���N�^
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const Vector2I& size, vk::Format format);

		/// <summary>
		/// �o�b�N�o�b�t�@
		/// </summary>
		/// <param name="image"></param>
		VkImageResource(const Vector2I& size, Format format,vk::Image& image, const vk::Device& device);

		struct Data
		{
			vk::UniqueDeviceMemory memory_;
			vk::UniqueImage image_;
			std::uint32_t arraySize_;
			std::uint32_t mipmapLevels_;
		};

	private:
		// ImageResource ����Čp������܂���
		bool CanMap(void) const final;
		void* GetResource(void) final;
		Vector2I GetSize(void) final;

		Data data_;
		Vector2I size_;
	};
}
