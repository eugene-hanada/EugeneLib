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

		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <returns> ビューのポインタ </returns>
		void* GetViews() noexcept
		{
			return &imageViews_;
		}

		// DepthStencilViews を介して継承されました
		void Create(ImageResource& resource, std::uint32_t idx);

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="views"></param>
		DepthStencilViews(DepthStencilViews&& views) noexcept :
			imageViews_{std::move(views.imageViews_)}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="views"></param>
		/// <returns></returns>
		DepthStencilViews& operator=(DepthStencilViews&& views) noexcept
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
		std::vector<vk::UniqueImageView> imageViews_;

		friend class Graphics;
	};
}