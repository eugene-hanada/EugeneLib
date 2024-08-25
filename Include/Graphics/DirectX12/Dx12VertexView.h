#pragma once
#include <d3d12.h>
#include <cstdint>
#include "Dx12BufferResource.h"

namespace Eugene
{
	/// <summary>
	/// 頂点ビュー
	/// </summary>
	class VertexView
	{
	public:
		VertexView() = default;
		
		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		void* GetView(void) noexcept
		{
			return &bufferView_;
		}

		/// <summary>
		/// 頂点数を取得する
		/// </summary>
		/// <returns></returns>
		std::uint32_t GetVertexNum() const noexcept 
		{
			return bufferView_.SizeInBytes / bufferView_.StrideInBytes;
		}

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="size"> 一つ当たりのサイズ(バイト数) </param>
		/// <param name="vertexNum"> 頂点数 </param>
		/// <param name="resource"> 頂点バッファのリソース </param>
		VertexView(std::uint32_t size, std::uint32_t vertexNum, BufferResource& resource) noexcept:
			bufferView_{ static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress(), size * vertexNum, size }
		{
		}

		/// <summary>
		/// コピーコンストラクタ
		/// </summary>
		/// <param name="vertexView"></param>
		VertexView(const VertexView& vertexView) noexcept :
			bufferView_{ vertexView.bufferView_ }
		{
		}

		/// <summary>
		/// コピー演算子
		/// </summary>
		/// <param name="vertexView"></param>
		/// <returns></returns>
		VertexView& operator =(const VertexView & vertexView) noexcept
		{
			bufferView_ = vertexView.bufferView_;
			return *this;
		}

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="size"> 一つ当たりのサイズ(バイト数) </param>
		/// <param name="vertexNum"> 頂点数 </param>
		/// <param name="resource"> 頂点バッファのリソース </param>
		void Init(std::uint32_t size, std::uint32_t vertexNum, BufferResource& resource) noexcept
		{
			bufferView_ = { static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress(), size * vertexNum, size };
		}
	private:

		D3D12_VERTEX_BUFFER_VIEW bufferView_;

		friend class Graphics;
	};
}

