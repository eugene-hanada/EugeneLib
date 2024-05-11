#pragma once
#include "../../../Include/Graphics/ShaderResourceViews.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Utils//ArgsSpan.h"
#include "../../../Include/Graphics/ResourceBindLayout.h"
#include <map>

namespace Eugene
{
	class VkShaderResourceViews :
		public ShaderResourceViews
	{
	public:
		VkShaderResourceViews(const vk::Device& device, const ArgsSpan<Bind>& ViewTypes);
		~VkShaderResourceViews();

		/// <summary>
		/// データ
		/// </summary>
		struct Data
		{
			/// <summary>
			/// ディスクリプタセット
			/// </summary>
			vk::UniqueDescriptorSet descriptorSet_;

			/// <summary>
			/// ディスクリプタセットのレイアウト
			/// </summary>
			vk::UniqueDescriptorSetLayout layout_;
		};
	private:

		// ShaderResourceViews を介して継承されました
		void CreateTexture(ImageResource& resource, std::uint64_t idx) final;
		void CreateConstantBuffer(BufferResource& resource, std::uint64_t idx) final;
		void CreateCubeMap(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) final;
		std::uint64_t GetImg(void) final;
		
		/// <summary>
		/// ディスクリプタプール
		/// </summary>
		vk::UniqueDescriptorPool descriptorPool_;

		/// <summary>
		/// データ
		/// </summary>
		Data data_;
		
		/// <summary>
		/// インデックスごとのビュータイプとbindingと配列の要素数
		/// </summary>
		std::vector<std::tuple<ViewType, std::uint32_t, std::uint32_t>> typeData_;

		/// <summary>
		/// ImageViewのマップ
		/// </summary>
		std::map<std::uint64_t, vk::UniqueImageView> imageViewMap_;
	};
}