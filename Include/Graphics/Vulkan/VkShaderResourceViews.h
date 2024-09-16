#pragma once
#include "../GraphicsCommon.h"
#include <vulkan/vulkan.hpp>
#include "../../Utils/ArgsSpan.h"
#include <map>

namespace Eugene
{
	class ImageResource;
	class BufferResource;

	/// <summary>
	/// シェーダリソースビュー
	/// </summary>
	class ShaderResourceViews
	{
	public:
		/// <summary>
		/// データ
		/// </summary>
		struct Data
		{
			Data() = default;

			Data(Data&& data) noexcept :
				descriptorSet_{ std::move(data.descriptorSet_) }, layout_{ std::move(data.layout_) }
			{
			}

			Data& operator=(Data&& data) noexcept
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
		ShaderResourceViews() = default;
		~ShaderResourceViews() = default;

		/// <summary>
		/// テクスチャ用のビューを生成する
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		void CreateTexture(ImageResource& resource, std::uint32_t idx);

		/// <summary>
		/// 定数バッファ用のビューを生成する
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		void CreateConstantBuffer(BufferResource& resource, std::uint32_t idx) ;

		/// <summary>
		/// キューブマップ用のビューを生成する
		/// </summary>
		/// <param name="resource"></param>
		/// <param name="idx"></param>
		void CreateCubeMap(ImageResource& resource, std::uint32_t idx) ;

		/// <summary>
		/// Unorderedで使用するバッファ用のビューを生成する
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		/// <param name="numElements"> 要素の数 </param>
		/// <param name="strideSize"> 要素一つ当たりのサイズ </param>
		void CreateUnorderedAccessBuffer(BufferResource& resource, std::uint32_t idx, std::uint32_t numElements, std::uint64_t strideSize);

		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		void* GetViews() noexcept
		{
			return &data_;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
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

		/// <summary>
		/// サイズ(保持できるビューの数)
		/// </summary>
		/// <returns></returns>
		std::uint32_t GetSize() const noexcept
		{
			return static_cast<std::uint32_t>(typeData_.size());
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
		Data data_;
		
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