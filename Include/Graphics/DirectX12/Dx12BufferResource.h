#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "../../ThirdParty/D3D12MemoryAllocator/include/D3D12MemAlloc.h"
#include "../../../ThirdParty/glm/glm/vec2.hpp"
#include "../GraphicsCommon.h"


namespace D3D12MA
{
	struct Allocation;
	struct Allocator;
}


namespace Eugene
{
	class Image;

	/// <summary>
	/// グラフィックスAPI用のバッファリソース
	/// </summary>
	class BufferResource
	{
	public:
		BufferResource() noexcept = default;
		BufferResource(std::uint64_t size, bool isUnordered, GpuResourceType type);
		BufferResource(Image& image);
		~BufferResource() = default;

		/// <summary>
		/// マップ可能か？
		/// </summary>
		/// <returns> マップ可能な場合true不可能な場合false </returns>
		bool CanMap(void) const noexcept
		{
			return canMap_;
		}
		
		/// <summary>
		/// API側のリソースの取得
		/// </summary>
		/// <returns> リソースのポインタ </returns>
		void* GetResource(void) noexcept
		{
			return resource_.Get();
		}

		/// <summary>
		/// サイズを取得する(バイト数)
		/// </summary>
		/// <returns> サイズ(バイト数) </returns>
		std::uint64_t GetSize(void) const
		{
			return resource_->GetDesc().Width;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final()noexcept
		{
			resource_.Reset();
			allocation_.Reset();
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="bufferResource"></param>
		BufferResource(BufferResource&& bufferResource) noexcept :
			resource_{std::move(bufferResource.resource_)}, allocation_{std::move(bufferResource.allocation_)}, canMap_{bufferResource.canMap_}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="bufferResource"></param>
		/// <returns></returns>
		BufferResource& operator=(BufferResource&& bufferResource) noexcept
		{
			resource_ = std::move(bufferResource.resource_);
			allocation_ = std::move(bufferResource.allocation_);
			canMap_ = bufferResource.canMap_;
		}

		/// <summary>
		/// マップする
		/// </summary>
		/// <returns> ポインタ </returns>
		void* Map() noexcept
		{
			void* ptr{ nullptr };
			resource_->Map(0, nullptr, &ptr);
			return ptr;
		}

		/// <summary>
		/// アンマップする
		/// </summary>
		void UnMap() noexcept
		{
			resource_->Unmap(0, nullptr);
		}
	private:
		BufferResource(const BufferResource&) = delete;
		BufferResource& operator=(const BufferResource&) = delete;

		/// <summary>
		/// アロケーターで確保したメモリ
		/// </summary>
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation_;

		/// <summary>
		/// リソース
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

		/// <summary>
		/// マップ可能か？
		/// </summary>
		bool canMap_;

	};
}
