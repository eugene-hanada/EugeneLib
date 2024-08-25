#pragma once
#include <vulkan/vulkan.hpp>
#include "../../../Include/Math/Math.h"

namespace Eugene
{
	class ImageResource;

	/// <summary>
	/// レンダーターゲット用のビュー
	/// </summary>
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

			Data(Data&& data) noexcept :
				imageView{ std::move(data.imageView) }, size{ data.size }
			{
			}

			Data& operator=(Data&& data) noexcept
			{
				imageView = std::move(data.imageView);
				size = data.size;
				return *this;
			}

			Data() = default;
		};

		/// <summary>
		/// ビューを生成する
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		void Create(ImageResource& resource, std::uint32_t idx);

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			imageViews_.clear();
		}

		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		void* GetViews(void) noexcept
		{
			return &imageViews_;
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="views"></param>
		RenderTargetViews(RenderTargetViews&& views) noexcept :
			imageViews_{std::move(views.imageViews_)}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="views"></param>
		/// <returns></returns>
		RenderTargetViews& operator=(RenderTargetViews&& views) noexcept
		{
			imageViews_ = std::move(views.imageViews_);
			return *this;
		}
		RenderTargetViews() = default;
		RenderTargetViews(const RenderTargetViews&) = delete;
		RenderTargetViews& operator=(const RenderTargetViews&) = delete;
	private:

		RenderTargetViews(std::uint32_t size);

		/// <summary>
		/// ビューのデータ
		/// </summary>
		std::vector<Data> imageViews_;

		friend class Graphics;
	};
}