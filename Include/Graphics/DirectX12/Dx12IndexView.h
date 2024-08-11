#pragma once
#include <d3d12.h>
#include <utility>
#include "../GraphicsCommon.h"
#include "Dx12BufferResource.h"

struct D3D12_INDEX_BUFFER_VIEW;

namespace Eugene
{
	class IndexView
	{
	public:
		IndexView() noexcept :
			indexNum_{0u}
		{
		}

		IndexView(std::uint32_t size, std::uint32_t indexNum, IndexType format, BufferResource& resource) noexcept :
			bufferView_{
			static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress() ,
			indexNum * static_cast<std::uint32_t>(indexTypeToSize[std::to_underlying(format)]),
			static_cast<DXGI_FORMAT>(format)
		},
			indexNum_{indexNum}
		{
		}

		void Init(std::uint32_t size, std::uint32_t indexNum, IndexType format, BufferResource& resource) noexcept
		{
			bufferView_ = {
			static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress() ,
			indexNum * static_cast<std::uint32_t>(indexTypeToSize[std::to_underlying(format)]),
				static_cast<DXGI_FORMAT>(format)
			};
			indexNum_ = indexNum;
		}

		void* GetView(void) noexcept
		{
			return &bufferView_;
		}

		std::uint32_t GetIndexNum() const noexcept
		{
			return indexNum_;
		}
	private:
		static constexpr uint32_t indexTypeToSize[]
		{
			4u,
			2u,
			1u
		};

		/// <summary>
		/// インデックスバッファービュー
		/// </summary>
		D3D12_INDEX_BUFFER_VIEW bufferView_;

		std::uint32_t indexNum_;
	};
}
