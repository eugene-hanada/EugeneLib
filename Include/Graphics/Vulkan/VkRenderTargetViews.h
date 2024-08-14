#pragma once
#include <vulkan/vulkan.hpp>
#include "../../../Include/Math/Math.h"

namespace Eugene
{
	class ImageResource;
	class RenderTargetViews
	{
	public:
		/// <summary>
		/// データ
		/// </summary>
		struct Data
		{
			/// <summary>
			/// ImageView
			/// </summary>
			vk::UniqueImageView imageView;

			/// <summary>
			/// 画像サイズ
			/// </summary>
			glm::ivec2 size{ 0,0 };
		};

		using ViewsType = std::vector<Data>;

		RenderTargetViews() = default;

		// RenderTargetViews を介して継承されました
		void Create(ImageResource& resource, std::uint32_t idx);

		void Final() noexcept
		{
			imageViews_.clear();
		}

		ViewsType& GetViews(void) noexcept
		{
			return imageViews_;
		}
	private:

		RenderTargetViews(std::uint32_t size);

		/// <summary>
		/// ビューのデータ
		/// </summary>
		ViewsType imageViews_;

		friend class Graphics;
	};
}