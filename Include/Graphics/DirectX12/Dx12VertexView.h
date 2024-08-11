#pragma once
#include <d3d12.h>
#include <cstdint>
#include "Dx12BufferResource.h"

namespace Eugene
{
	class VertexView
	{
	public:
		VertexView() = default;
		
		void* GetView(void) noexcept
		{
			return &bufferView_;
		}

		std::uint32_t GetVertexNum() const noexcept 
		{
			return bufferView_.SizeInBytes / bufferView_.StrideInBytes;
		}

		VertexView(std::uint32_t size, std::uint32_t vertexNum, BufferResource& resource) noexcept:
			bufferView_{ static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress(), size * vertexNum, size }
		{
		}

		VertexView(const VertexView& vertexView) noexcept :
			bufferView_{ vertexView.bufferView_ }
		{
		}

		VertexView& operator =(const VertexView & vertexView) noexcept
		{
			bufferView_ = vertexView.bufferView_;
			return *this;
		}

		void Init(std::uint32_t size, std::uint32_t vertexNum, BufferResource& resource) noexcept
		{
			bufferView_ = { static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress(), size * vertexNum, size };
		}
	private:

		D3D12_VERTEX_BUFFER_VIEW bufferView_;

		friend class Graphics;
	};
}

