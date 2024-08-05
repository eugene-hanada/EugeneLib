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

	class ImageResource
	{
	public:
		ImageResource() noexcept :
			format_{Format::NON}
		{
		}

		ImageResource(ImageResource&& imageResource) noexcept :
			allocation_{std::move(imageResource.allocation_)}, resource_{std::move(imageResource.resource_)}, format_{imageResource.format_}
		{
			imageResource.format_ = Format::NON;
		}

		ImageResource& operator=(ImageResource&& imageResource) noexcept
		{
			allocation_ = std::move(imageResource.allocation_);
			resource_ = std::move(imageResource.resource_);
			format_ = imageResource.format_;
			imageResource.format_ = Format::NON;
			return *this;
		}

		ImageResource(const TextureInfo& info);
		ImageResource(
			const glm::ivec2& size,
			Format format,
			std::uint32_t arraySize,
			std::uint8_t mipLeveles,
			std::uint8_t sampleCount,
			std::optional<std::span<float, 4>> clearColor);

		ImageResource(const glm::ivec2& size, Format format, float clearValue, std::uint8_t sampleCount);

		glm::ivec2 GetSize(void) const
		{
			auto desc{ resource_->GetDesc() };
			return { static_cast<int>(desc.Width),static_cast<int>(desc.Height) };
		}

		virtual bool CanMap(void)const noexcept
		{
			return false;
		}

		void* GetResource(void) const
		{
			return resource_.Get();
		}

		void Final() noexcept
		{
			resource_.Reset();
			allocation_.Reset();
			format_ = Format::NON;
		}

	private:
		ImageResource(std::uint32_t idx);
		ImageResource(const ImageResource&) = delete;
		ImageResource& operator=(const ImageResource&) = delete;
		/// <summary>
		/// �t�H�[�}�b�g
		/// </summary>
		Format format_;

		/// <summary>
		/// �A���P�[�^�[�Ŋm�ۂ���������
		/// </summary>
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation_;

		/// <summary>
		/// ���\�[�X
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

		friend class Graphics;
	};
}