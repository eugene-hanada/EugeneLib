#pragma once
#include <vulkan/vulkan.hpp>
#include "../../../Include/Graphics/ImageResource.h"

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
		VkImageResource(const VkGraphics& graphics,const vk::Device& device, const TextureInfo& info);

		/// <summary>
		/// デブスバッファの場合のコンストラクタ
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		/// <param name="clearValue"></param>
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const glm::ivec2& size, float clearValue);


		/// <summary>
		/// レンダーターゲットの場合のコンストラクタ
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		/// <param name="format"></param>
		/// <param name="clearColor"></param>
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const glm::ivec2& size, Format format);


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
			/// デバイスメモリ
			/// </summary>
			vk::UniqueDeviceMemory memory_;

			/// <summary>
			/// Image
			/// </summary>
			vk::UniqueImage image_;

			/// <summary>
			/// 画像配列サイズ
			/// </summary>
			std::uint32_t arraySize_;

			/// <summary>
			/// ミップマップレベル
			/// </summary>
			std::uint32_t mipmapLevels_;
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
