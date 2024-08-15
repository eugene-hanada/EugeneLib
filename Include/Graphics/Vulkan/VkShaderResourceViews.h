#pragma once
#include "../GraphicsCommon.h"
#include <vulkan/vulkan.hpp>
#include "../../Utils/ArgsSpan.h"
#include <map>

namespace Eugene
{
	class ImageResource;
	class BufferResource;
	class ShaderResourceViews
	{
		/// <summary>
		/// データ
		/// </summary>
		struct Data
		{
			Data() = default;

			Data(Data&& data) :
				descriptorSet_{std::move(data.descriptorSet_)}, layout_{std::move(data.layout_)}
			{
			}

			Data& operator=(Data&& data)
			{
				descriptorSet_ = std::move(data.descriptorSet_);
				layout_ = std::move(data.layout_);
			}

			/// <summary>
			/// ディスクリプタセット
			/// </summary>
			vk::UniqueDescriptorSet descriptorSet_;

			/// <summary>
			/// ディスクリプタセットのレイアウト
			/// </summary>
			vk::UniqueDescriptorSetLayout layout_;
		};
	public:
		using ViewsType = Data;
		ShaderResourceViews() = default;
		~ShaderResourceViews() = default;

		// ShaderResourceViews を介して継承されました
		void CreateTexture(ImageResource& resource, std::uint32_t idx);
		void CreateConstantBuffer(BufferResource& resource, std::uint32_t idx) ;
		void CreateCubeMap(ImageResource& resource, std::uint32_t idx) ;
		void CreateUnorderedAccessBuffer(BufferResource& resource, std::uint32_t idx, std::uint32_t numElements, std::uint64_t strideSize);
		std::uint64_t GetImg(void) ;

		ViewsType& GetViews() noexcept
		{
			return data_;
		}

		void Final() noexcept
		{
			imageViewMap_.clear();
			typeData_.clear();
			data_.layout_.reset();
			data_.descriptorSet_.reset();
			descriptorPool_.reset();
		}

		ShaderResourceViews(ShaderResourceViews&& views) noexcept :
			descriptorPool_{std::move(views.descriptorPool_)}, data_{std::move(views.data_)},
			typeData_{std::move(views.typeData_)}, imageViewMap_{std::move(views.imageViewMap_)}
		{

		}
		ShaderResourceViews& operator=(ShaderResourceViews&& views) noexcept
		{
			descriptorPool_ = std::move(views.descriptorPool_);
			data_ = std::move(views.data_);
			typeData_ = std::move(views.typeData_);
			imageViewMap_ = std::move(views.imageViewMap_);
		}

		ShaderResourceViews(const ShaderResourceViews&) = delete;
		ShaderResourceViews& operator=(const ShaderResourceViews&) = delete;
	private:

		ShaderResourceViews(const ArgsSpan<Bind>& ViewTypes);

		
		
		/// <summary>
		/// ディスクリプタプール
		/// </summary>
		vk::UniqueDescriptorPool descriptorPool_;

		/// <summary>
		/// データ
		/// </summary>
		ViewsType data_;
		
		/// <summary>
		/// インデックスごとのビュータイプとbindingと配列の要素数
		/// </summary>
		std::vector<std::tuple<ViewType, std::uint32_t, std::uint32_t>> typeData_;

		/// <summary>
		/// ImageViewのマップ
		/// </summary>
		std::map<std::uint64_t, vk::UniqueImageView> imageViewMap_;

		friend class Graphics;
	};
}