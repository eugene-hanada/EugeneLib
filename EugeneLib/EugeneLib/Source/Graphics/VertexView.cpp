#include "../../Include/Graphics/VertexView.h"
#include "DirectX12/Dx12VertexView.h"

EugeneLib::VertexView::~VertexView()
{
}

EugeneLib::VertexView::VertexView()
{
}

EugeneLib::VertexView* EugeneLib::CreateVertexView(size_t size, size_t vertexNum, GpuResource& resource)
{
	return new Dx12VertexView{size,vertexNum,resource};
}
