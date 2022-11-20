#include "Dx12VertexView.h"
#include <d3d12.h>
#include "../../../Include/Graphics/GpuResource.h"

EugeneLib::Dx12VertexView::Dx12VertexView(size_t size, size_t vertexNum, GpuResource& resource) :
	VertexView{}
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	bufferView_ = std::make_unique<D3D12_VERTEX_BUFFER_VIEW>();
	bufferView_->BufferLocation = dx12Resource->GetGPUVirtualAddress();
	bufferView_->SizeInBytes = static_cast<std::uint32_t>(size * vertexNum);
	bufferView_->StrideInBytes = static_cast<std::uint32_t>(size);
	
}

EugeneLib::Dx12VertexView::~Dx12VertexView()
{
}

void* EugeneLib::Dx12VertexView::GetView(void) const
{
	return bufferView_.get();
}
