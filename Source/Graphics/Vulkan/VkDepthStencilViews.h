#pragma once
#include "../../../Include/Graphics/DepthStencilViews.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	/// <summary>
	/// �[�x�o�b�t�@�p�r���[
	/// </summary>
	class  VkDepthStencilView :
		public DepthStencilViews
	{
	public:
		VkDepthStencilView(const vk::Device& device, size_t size);
	private:
		// DepthStencilViews ����Čp������܂���
		void Create(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) final;

		/// <summary>
		/// ImageViews��std::vector
		/// </summary>
		std::vector<vk::UniqueImageView> imageViews_;
	};
}