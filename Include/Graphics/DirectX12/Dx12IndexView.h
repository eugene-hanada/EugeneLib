#pragma once
#include <d3d12.h>
#include <utility>
#include "../GraphicsCommon.h"
#include "Dx12BufferResource.h"

struct D3D12_INDEX_BUFFER_VIEW;

namespace Eugene
{
	/// <summary>
	/// インデックスビュー
	/// </summary>
	class IndexView
	{
	public:

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IndexView() noexcept :
			bufferView_{}, indexNum_{ 0u }
		{
		}

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="size"> サイズ(バイト数) </param>
		/// <param name="indexNum"> インデックスの数 </param>
		/// <param name="type"> インデックスのタイプ </param>
		/// <param name="resource"> インデックスバッファのリソース </param>
		IndexView(std::uint32_t size, std::uint32_t indexNum, IndexType type, BufferResource& resource) noexcept :
			bufferView_{
			static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress() ,
			indexNum * static_cast<std::uint32_t>(indexTypeToSize[std::to_underlying(type)]),
			static_cast<DXGI_FORMAT>(type)
		},
			indexNum_{indexNum}
		{
		}

		/// <summary>
		/// 遅延初期用の関数
		/// </summary>
		/// <param name="size"> サイズ(バイト数) </param>
		/// <param name="indexNum"> インデックスの数 </param>
		/// <param name="type"> インデックスのタイプ </param>
		/// <param name="resource">  </param>
		void Init(std::uint32_t size, std::uint32_t indexNum, IndexType format, BufferResource& resource) noexcept
		{
			bufferView_ = {
			static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress() ,
			indexNum * static_cast<std::uint32_t>(indexTypeToSize[std::to_underlying(format)]),
				static_cast<DXGI_FORMAT>(format)
			};
			indexNum_ = indexNum;
		}

		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> ビューのポインタ </returns>
		void* GetView(void) noexcept
		{
			return &bufferView_;
		}

		/// <summary>
		/// インデックス数を取得する
		/// </summary>
		/// <returns></returns>
		std::uint32_t GetIndexNum() const noexcept
		{
			return indexNum_;
		}
	private:

		/// <summary>
		/// インデックスタイプからインデックスのサイズ(バイト数)にするための定数配列
		/// </summary>
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

		/// <summary>
		/// インデックス数
		/// </summary>
		std::uint32_t indexNum_;
	};
}
