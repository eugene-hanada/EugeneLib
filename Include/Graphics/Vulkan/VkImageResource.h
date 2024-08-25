#pragma once
#include "../GraphicsCommon.h"
#include "../../ThirdParty/glm/glm/vec2.hpp"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>
#include <optional>
#include <span>

namespace Eugene
{
	struct TextureInfo;

	/// <summary>
	/// 画像用のリソース
	/// </summary>
	class ImageResource
	{
	public:
		struct ImageData
		{
			ImageData()
			{
			}

			ImageData(ImageData&& imageData) noexcept :
				image_{ std::move(imageData.image_) }, arraySize_{ imageData.arraySize_ }, mipmapLevels_{ imageData.mipmapLevels_ }, pixelPerSize_{ imageData.pixelPerSize_ }
			{
			}

			ImageData& operator=(ImageData&& imageData) noexcept
			{
				image_ = std::move(imageData.image_);
				arraySize_ = imageData.arraySize_;
				mipmapLevels_ = imageData.mipmapLevels_;
				pixelPerSize_ = imageData.pixelPerSize_;
				return *this;
			}

			/// <summary>
			/// Image
			/// </summary>
			vma::UniqueImage image_;

			/// <summary>
			/// 画像配列サイズ
			/// </summary>
			std::uint32_t arraySize_{ 0u };

			/// <summary>
			/// ミップマップレベル
			/// </summary>
			std::uint8_t mipmapLevels_{ 0u };

			/// <summary>
			/// 1ピクセル当たりのサイズ
			/// </summary>
			std::uint8_t pixelPerSize_{ 0u };
		};

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ImageResource() noexcept :
			canMap_{false}, isBackBuffer_{false}, format_{Format::NON}, size_{0,0}
		{
		}

		/// <summary>
		/// デストラクタ
		/// </summary>
		~ImageResource() noexcept;

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final()
		{
			if (!isBackBuffer_)
			{
				imageData_.image_.reset();
			}
			allocation_.reset();
		}

		/// <summary>
		/// マップ可能か？
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		bool CanMap(void) const noexcept
		{
			return canMap_;
		}


		/// <summary>
		/// API側のリソースを取得
		/// </summary>
		/// <param name=""></param>
		/// <returns> リソースのポインタ </returns>
		void* GetResource(void) noexcept
		{
			return &imageData_;
		}

		/// <summary>
		/// サイズを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		glm::ivec2 GetSize(void) const noexcept
		{
			return size_;
		}

		/// <summary>
		/// フォーマットを取得する
		/// </summary>
		/// <returns></returns>
		Format GetFormat() const noexcept
		{
			return format_;
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="imageResource"></param>
		ImageResource(ImageResource&& imageResource) noexcept :
			allocation_{std::move(imageResource.allocation_)}, imageData_{std::move(imageResource.imageData_)},
			size_{imageResource.size_}, canMap_{imageResource.canMap_}, isBackBuffer_{imageResource.isBackBuffer_}, format_{imageResource.format_}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="imageResource"></param>
		/// <returns></returns>
		ImageResource& operator=(ImageResource&& imageResource) noexcept
		{
			isBackBuffer_ = imageResource.isBackBuffer_;
			allocation_ = std::move(imageResource.allocation_);
			

			if (!isBackBuffer_)
			{
				imageData_ = std::move(imageResource.imageData_);
			}
			else
			{
				imageData_.image_.release();
				imageData_.image_.reset(*imageResource.imageData_.image_);
				imageData_.arraySize_ = imageResource.imageData_.arraySize_;
				imageData_.mipmapLevels_ = imageResource.imageData_.mipmapLevels_;
				imageData_.pixelPerSize_ = imageResource.imageData_.pixelPerSize_;
			}
			

			size_ = imageResource.size_;
			isBackBuffer_ = imageResource.isBackBuffer_;
			canMap_ = imageResource.canMap_;
			format_ = imageResource.format_;
			return *this;
		}

		ImageResource(const ImageResource&) = delete;
		ImageResource& operator=(const ImageResource&) = delete;
	private:
		

		/// <summary>
		/// テクスチャ用のコンストラクタ
		/// </summary>
		/// <param name="info"> テクスチャの情報 </param>
		ImageResource(const TextureInfo& info);

		/// <summary>
		/// デブスバッファの場合のコンストラクタ
		/// </summary>
		/// <param name="size"></param>
		/// <param name="clearValue"></param>
		ImageResource(const glm::ivec2& size, float clearValue, std::uint8_t sampleCount);



		
		/// <summary>
		/// レンダーターゲットの場合のコンストラクタ
		/// </summary>
		/// <param name="size"></param>
		/// <param name="format"></param>
		/// <param name="arraySize"></param>
		/// <param name="mipLeveles"></param>
		/// <param name="sampleCount"></param>
		/// <param name="clearColor"></param>
		ImageResource(
			const glm::ivec2& size,
			Format format,
			std::uint32_t arraySize,
			std::uint8_t mipLeveles,
			std::uint8_t sampleCount,
			std::optional<std::span<float, 4>> clearColor
		);


		/// <summary>
		/// バックバッファ
		/// </summary>
		/// <param name="image"></param>
		ImageResource(const glm::ivec2& size, Format format, vk::Image& image);

		/// <summary>
		/// アロケーターで確保したメモリ
		/// </summary>
		vma::UniqueAllocation allocation_;

		ImageData imageData_;

		/// <summary>
		/// 画像サイズ
		/// </summary>
		glm::ivec2 size_;

		Format format_;


		/// <summary>
		/// バックバッファか？
		/// </summary>
		bool isBackBuffer_{false};

		/// <summary>
		/// 
		/// </summary>
		bool canMap_;

		friend class Graphics;
	};
}
