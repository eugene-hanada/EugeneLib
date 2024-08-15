#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace Eugene
{
	/// <summary>
	/// 深度バッファ用ビュー
	/// </summary>
	class DepthStencilViews
	{
	public:
		using ViewsType = std::vector<vk::UniqueImageView>;

		ViewsType& GetViews() noexcept
		{
			return imageViews_;
		}

		// DepthStencilViews を介して継承されました
		void Create(ImageResource& resource, std::uint32_t idx);


		DepthStencilViews(DepthStencilViews&& views) :
			imageViews_{std::move(views.imageViews_)}
		{
		}

		DepthStencilViews& operator=(DepthStencilViews&& views)
		{
			imageViews_ = std::move(views.imageViews_);
		}

		DepthStencilViews(const DepthStencilViews&) = delete;
		DepthStencilViews& operator=(const DepthStencilViews&) = delete;
	private:

		DepthStencilViews(std::uint32_t size);



		/// <summary>
		/// ImageViewsのstd::vector
		/// </summary>
		ViewsType imageViews_;

		friend class Graphics;
	};
}