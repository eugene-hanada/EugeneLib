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
	class ImageResource
	{
		struct ImageData
		{
			ImageData()
			{
			}

			ImageData(ImageData&& imageData) :
				image_{std::move(imageData.image_)}, arraySize_{imageData.arraySize_}, mipmapLevels_{imageData.mipmapLevels_}, pixelPerSize_{imageData.pixelPerSize_}
			{
			}

			ImageData& operator=(ImageData&& imageData)
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
	public:
		using ResourceType = ImageData;
		ImageResource() noexcept :
			canMap_{false}
		{
		}

		~ImageResource() noexcept;

		void Final()
		{
			imageData_.image_.release();
			allocation_.release();
		}

		bool CanMap(void) const noexcept
		{
			return canMap_;
		}

		ResourceType& GetResource(void) noexcept
		{
			return imageData_;
		}

		glm::ivec2 GetSize(void) const noexcept
		{
			return size_;
		}

		Format GetFormat() const noexcept
		{
			return format_;
		}


		ImageResource(ImageResource&& imageResource) noexcept :
			allocation_{std::move(imageResource.allocation_)}, imageData_{std::move(imageResource.imageData_)},
			size_{imageResource.size_}, canMap_{imageResource.canMap_}, isBackBuffer_{imageResource.isBackBuffer_}, format_{imageResource.format_}
		{
		}

		ImageResource& operator=(ImageResource&& imageResource) noexcept
		{
			allocation_ = std::move(imageResource.allocation_);
			imageData_ = std::move(imageResource.imageData_);
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

		ResourceType imageData_;

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
