#pragma once
#include <vulkan/vulkan.hpp>
#include "VkBufferResource.h"

namespace Eugene
{
	/// <summary>
	/// 頂点ビュー
	/// </summary>
	class VertexView
	{
	public:
		/// <summary>
		/// 頂点数を取得する
		/// </summary>
		/// <returns></returns>
		std::uint32_t GetVertexNum() const noexcept
		{
			return vertexNum_;
		}

		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		void* GetView() noexcept
		{
			return &buffer_;
		}

		VertexView() noexcept :
			vertexNum_{0}
		{
		}

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="size"> 一つ当たりのサイズ(バイト数) </param>
		/// <param name="vertexNum"> 頂点数 </param>
		/// <param name="resource"> 頂点バッファのリソース </param>
		VertexView(std::uint32_t size, std::uint32_t vertexNum, BufferResource& resource) noexcept :
			buffer_{ *static_cast<vk::Buffer*>(resource.GetResource()) }, vertexNum_{ vertexNum }
		{
		}

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="size"> 一つ当たりのサイズ(バイト数) </param>
		/// <param name="vertexNum"> 頂点数 </param>
		/// <param name="resource"> 頂点バッファのリソース </param>
		void Init(std::uint32_t size, std::uint32_t vertexNum, BufferResource& resource)
		{
			buffer_ = *static_cast<vk::Buffer*>(resource.GetResource());
			vertexNum_ = vertexNum;
		}

		/// <summary>
		/// コピーコンストラクタ
		/// </summary>
		/// <param name="view"></param>
		VertexView(const VertexView& view) noexcept :
			buffer_{view.buffer_}, vertexNum_{view.vertexNum_}
		{
		}

		/// <summary>
		/// コピー演算子
		/// </summary>
		/// <param name="view"></param>
		/// <returns></returns>
		VertexView& operator=(const VertexView& view) noexcept
		{
			buffer_ = view.buffer_;
			vertexNum_ = view.vertexNum_;
		}
	private:
		
	
		/// <summary>
		/// バッファ
		/// </summary>
		vk::Buffer buffer_;

		/// <summary>
		/// 
		/// </summary>
		std::uint32_t vertexNum_;
	};
}

