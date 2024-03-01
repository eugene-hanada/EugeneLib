#pragma once
#include <vulkan/vulkan.hpp>
#include "../../../Include/Graphics/ImageResource.h"
#include "../../../Include/ThirdParty/VulkanMemoryAllocator-Hpp/include/vk_mem_alloc.hpp"

namespace Eugene
{
	class VkGraphics;
	class VkImageResource :
		public ImageResource
	{
	public:
		/// <summary>
		/// テクスチャ用のコンストラクタ
		/// </summary>
		/// <param name="graphics"> グラフィックスクラス </param>
		/// <param name="device"> Vulkanのデバイス </param>
		/// <param name="info"> テクスチャの情報 </param>
		VkImageResource(const vma::Allocator& allocator, const TextureInfo& info);

		/// <summary>
		/// デブスバッファの場合のコンストラクタ
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		/// <param name="clearValue"></param>
		VkImageResource(const vma::Allocator& allocator, const glm::ivec2& size, float clearValue);


		/// <summary>
		/// レンダーターゲットの場合のコンストラクタ
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		/// <param name="format"></param>
		/// <param name="clearColor"></param>
		VkImageResource(const vma::Allocator& allocator, const glm::ivec2& size, Format format);


		/// <summary>
		/// バックバッファ
		/// </summary>
		/// <param name="image"></param>
		VkImageResource(const glm::ivec2& size, Format format,vk::Image& image, const vk::Device& device);

		~VkImageResource();

		/// <summary>
		/// データ
		/// </summary>
		struct Data
		{

			/// <summary>
			/// アロケーターで確保したメモリ
			/// </summary>
			vma::UniqueAllocation allocation_;


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
			std::uint32_t mipmapLevels_{ 0u };

			/// <summary>
			/// 1ピクセル当たりのサイズ
			/// </summary>
			std::uint32_t pixelPerSize{ 0u };
		};

	private:
		// ImageResource を介して継承されました
		bool CanMap(void) const final;
		void* GetResource(void) final;
		glm::ivec2 GetSize(void) final;

		/// <summary>
		/// データ
		/// </summary>
		Data data_;

		/// <summary>
		/// 画像サイズ
		/// </summary>
		glm::ivec2 size_;

		/// <summary>
		/// バックバッファか？
		/// </summary>
		bool isBackBuffer_{false};
	};
}
