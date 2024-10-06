#pragma once
#include <vulkan/vulkan.hpp>
#include  "VkBufferResource.h"

namespace Eugene
{
	class GpuResource;

	/// <summary>
	/// インデックスビュー
	/// </summary>
	class IndexView
	{
	public:

		/// <summary>
		/// インデックス数を取得する
		/// </summary>
		/// <returns></returns>
		std::uint32_t GetIndexNum() const noexcept
		{
			return indexNum_;
		}

		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> ビューのポインタ </returns>
		void* GetView() noexcept
		{
			return &buffer_;
		}

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IndexView() noexcept :
			indexNum_{ 0 }
		{
		}

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="size"> サイズ(バイト数) </param>
		/// <param name="indexNum"> インデックスの数 </param>
		/// <param name="type"> インデックスのタイプ </param>
		/// <param name="resource"> インデックスバッファのリソース </param>
		IndexView(std::uint32_t size, std::uint32_t indexNum, BufferResource& resource) noexcept :
			buffer_{*static_cast<vk::Buffer*>(resource.GetResource())}, indexNum_{indexNum}
		{
		}

		/// <summary>
		/// 遅延初期用の関数
		/// </summary>
		/// <param name="size"> サイズ(バイト数) </param>
		/// <param name="indexNum"> インデックスの数 </param>
		/// <param name="type"> インデックスのタイプ </param>
		/// <param name="resource">  </param>
		void Init(std::uint32_t size, std::uint32_t indexNum, BufferResource& resource)noexcept
		{
			buffer_ = *static_cast<vk::Buffer*>(resource.GetResource());
			indexNum_ = indexNum;
		}

		/// <summary>
		/// コピーコンストラクタ
		/// </summary>
		/// <param name="view"></param>
		IndexView(const IndexView& view) noexcept :
			buffer_{ view.buffer_ }, indexNum_{ view.indexNum_ }
		{
		}

		/// <summary>
		/// コピー演算子
		/// </summary>
		/// <param name="view"></param>
		/// <returns></returns>
		IndexView& operator=(const IndexView& view) noexcept
		{
			buffer_ = view.buffer_;
			indexNum_ = view.indexNum_;
			return *this;
		}
	private:
	
		vk::Buffer buffer_;
		std::uint32_t indexNum_;
	};
}