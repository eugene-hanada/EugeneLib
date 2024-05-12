#pragma once
#include "../../../Include/Graphics/DepthStencilViews.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	/// <summary>
	/// 深度バッファ用ビュー
	/// </summary>
	class  VkDepthStencilView :
		public DepthStencilViews
	{
	public:
		VkDepthStencilView(const vk::Device& device, size_t size);
	private:
		// DepthStencilViews を介して継承されました
		void Create(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) final;

		/// <summary>
		/// ImageViewsのstd::vector
		/// </summary>
		std::vector<vk::UniqueImageView> imageViews_;

		/// <summary>
		/// デバイスの参照
		/// </summary>
		const vk::Device& device_;
	};
}