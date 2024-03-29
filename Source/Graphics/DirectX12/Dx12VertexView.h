﻿#pragma once
#include "../../../Include/Graphics/VertexView.h"
#include <memory>

struct D3D12_VERTEX_BUFFER_VIEW;

namespace Eugene
{
	class BufferResource;
	class Dx12VertexView :
		public VertexView
	{
	public:
		Dx12VertexView(size_t size, size_t vertexNum, BufferResource& resource);
		~Dx12VertexView();

	private:
		void* GetView(void) final;
		std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> bufferView_;
	};
}

