#pragma once
#include <optional>
#include <span>
#include <wrl.h>
#include <d3d12.h>
#include "../../ThirdParty/D3D12MemoryAllocator/include/D3D12MemAlloc.h"
#include "../../../ThirdParty/glm/glm/vec2.hpp"
#include "../GraphicsCommon.h"

namespace Eugene
{
	class TextureInfo;

	/// <summary>
	/// 画像用のリソース
	/// </summary>
	class ImageResource
	{
	public:

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ImageResource() noexcept :
			format_{Format::NON}
		{
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="imageResource"></param>
		ImageResource(ImageResource&& imageResource) noexcept :
			allocation_{std::move(imageResource.allocation_)}, resource_{std::move(imageResource.resource_)}, format_{imageResource.format_}
		{
			imageResource.format_ = Format::NON;
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="imageResource"></param>
		/// <returns></returns>
		ImageResource& operator=(ImageResource&& imageResource) noexcept
		{
			allocation_ = std::move(imageResource.allocation_);
			resource_ = std::move(imageResource.resource_);
			format_ = imageResource.format_;
			imageResource.format_ = Format::NON;
			return *this;
		}

		/// <summary>
		/// サイズを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		glm::ivec2 GetSize(void) const
		{
			auto desc{ resource_->GetDesc() };
			return { static_cast<int>(desc.Width),static_cast<int>(desc.Height) };
		}

		/// <summary>
		/// マップ可能か？
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		bool CanMap(void)const noexcept
		{
			return false;
		}

		/// <summary>
		/// API側のリソースを取得
		/// </summary>
		/// <param name=""></param>
		/// <returns> リソースのポインタ </returns>
		void* GetResource(void) const
		{
			return resource_.Get();
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			resource_.Reset();
			allocation_.Reset();
			format_ = Format::NON;
		}

		/// <summary>
		/// フォーマットを取得する
		/// </summary>
		/// <returns></returns>
		Format GetFormat() const noexcept
		{
			return format_;
		}

	private:
		ImageResource(const TextureInfo& info);
		ImageResource(
			const glm::ivec2& size,
			Format format,
			std::uint32_t arraySize,
			std::uint8_t mipLeveles,
			std::uint8_t sampleCount,
			std::optional<std::span<float, 4>> clearColor);

		ImageResource(const glm::ivec2& size, Format format, float clearValue, std::uint8_t sampleCount);
		ImageResource(std::uint32_t idx);
		ImageResource(const ImageResource&) = delete;
		ImageResource& operator=(const ImageResource&) = delete;
		/// <summary>
		/// フォーマット
		/// </summary>
		Format format_;

		/// <summary>
		/// アロケーターで確保したメモリ
		/// </summary>
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation_;

		/// <summary>
		/// リソース
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

		friend class Graphics;
	};
}