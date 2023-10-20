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
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const Vector2I& size, float clearValue);


		/// <summary>
		/// レンダーターゲットの場合のコンストラクタ
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		/// <param name="format"></param>
		/// <param name="clearColor"></param>
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const Vector2I& size, Format format);

		/// <summary>
		/// バックバッファの場合のコンストラクタ
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="device"></param>
		/// <param name="size"></param>
		VkImageResource(const VkGraphics& graphics, const vk::Device& device, const Vector2I& size, vk::Format format);

		/// <summary>
		/// バックバッファ
		/// </summary>
		/// <param name="image"></param>
		VkImageResource(const Vector2I& size, Format format,vk::Image& image, const vk::Device& device);

		struct Data
		{
			vk::UniqueDeviceMemory memory_;
			vk::UniqueImage image_;
			std::uint32_t arraySize_;
			std::uint32_t mipmapLevels_;
		};

	private:
		// ImageResource を介して継承されました
		bool CanMap(void) const final;
		void* GetResource(void) final;
		Vector2I GetSize(void) final;

		Data data_;
		Vector2I size_;
	};
}
