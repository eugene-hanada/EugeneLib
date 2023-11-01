#pragma once
#include "../../../Include/Graphics/RenderTargetViews.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Math/Geometry.h"
#include <optional>

namespace Eugene
{
	class VkRenderTargetViews :
		public RenderTargetViews
	{
	public:
		
		VkRenderTargetViews(std::uint64_t size);

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
			glm::ivec2 size;
		};
		using ViewsType = std::vector<Data>;
	private:
		// RenderTargetViews を介して継承されました
		void Create(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) final;

		/// <summary>
		/// ビューのデータ
		/// </summary>
		ViewsType imageViews_;
	};
}