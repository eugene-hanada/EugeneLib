#pragma once
#include "../../../Include/Graphics/ShaderResourceViews.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Common/ArgsSpan.h"
#include "../../../Include/Graphics/ResourceBindLayout.h"
#include <map>

namespace Eugene
{
	class VkShaderResourceViews :
		public ShaderResourceViews
	{
	public:
		VkShaderResourceViews(const vk::Device& device, const ArgsSpan<Bind>& ViewTypes);
		~VkShaderResourceViews();

		/// <summary>
		/// �f�[�^
		/// </summary>
		struct Data
		{
			/// <summary>
			/// �f�B�X�N���v�^�Z�b�g
			/// </summary>
			vk::UniqueDescriptorSet descriptorSet_;

			/// <summary>
			/// �f�B�X�N���v�^�Z�b�g�̃��C�A�E�g
			/// </summary>
			vk::UniqueDescriptorSetLayout layout_;
		};
	private:

		// ShaderResourceViews ����Čp������܂���
		void CreateTexture(ImageResource& resource, std::uint64_t idx) final;
		void CreateConstantBuffer(BufferResource& resource, std::uint64_t idx) final;
		void CreateCubeMap(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) final;
		std::uint64_t GetImg(void) final;
		
		/// <summary>
		/// �f�B�X�N���v�^�v�[��
		/// </summary>
		vk::UniqueDescriptorPool descriptorPool_;

		/// <summary>
		/// �f�[�^
		/// </summary>
		Data data_;
		
		/// <summary>
		/// �C���f�b�N�X���Ƃ̃r���[�^�C�v��binding�Ɣz��̗v�f��
		/// </summary>
		std::vector<std::tuple<ViewType, std::uint32_t, std::uint32_t>> typeData_;

		/// <summary>
		/// ImageView�̃}�b�v
		/// </summary>
		std::map<std::uint64_t, vk::UniqueImageView> imageViewMap_;
	};
}