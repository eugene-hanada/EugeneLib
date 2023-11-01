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
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const glm::ivec2& size, float clearValue);


		/// <summary>
		/// �����_�[�^�[�Q�b�g�̏ꍇ�̃R���X�g���N�^
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		/// <param name="format"></param>
		/// <param name="clearColor"></param>
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const glm::ivec2& size, Format format);


		/// <summary>
		/// �o�b�N�o�b�t�@
		/// </summary>
		/// <param name="image"></param>
		VkImageResource(const glm::ivec2& size, Format format,vk::Image& image, const vk::Device& device);

		~VkImageResource();

		/// <summary>
		/// �f�[�^
		/// </summary>
		struct Data
		{
			/// <summary>
			/// �f�o�C�X������
			/// </summary>
			vk::UniqueDeviceMemory memory_;

			/// <summary>
			/// Image
			/// </summary>
			vk::UniqueImage image_;

			/// <summary>
			/// �摜�z��T�C�Y
			/// </summary>
			std::uint32_t arraySize_;

			/// <summary>
			/// �~�b�v�}�b�v���x��
			/// </summary>
			std::uint32_t mipmapLevels_;
		};

	private:
		// ImageResource ����Čp������܂���
		bool CanMap(void) const final;
		void* GetResource(void) final;
		glm::ivec2 GetSize(void) final;

		/// <summary>
		/// �f�[�^
		/// </summary>
		Data data_;

		/// <summary>
		/// �摜�T�C�Y
		/// </summary>
		glm::ivec2 size_;

		/// <summary>
		/// �o�b�N�o�b�t�@���H
		/// </summary>
		bool isBackBuffer_{false};
	};
}
