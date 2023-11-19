#include "Dx12VertexView.h"
#include <d3d12.h>
#include "../../../Include/Graphics/BufferResource.h"

Eugene::Dx12VertexView::Dx12VertexView(size_t size, size_t vertexNum, BufferResource& resource) :
	VertexView{static_cast<std::uint32_t>(vertexNum)}
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	bufferView_ = std::make_unique<D3D12_VERTEX_BUFFER_VIEW>();
	bufferView_->BufferLocation = dx12Resource->GetGPUVirtualAddress();
	bufferView_->SizeInBytes = static_cast<std::uint32_t>(size * vertexNum);
	bufferView_->StrideInBytes = static_cast<std::uint32_t>(size);
}

Eugene::Dx12VertexView::~Dx12VertexView()
{
}

void* Eugene::Dx12VertexView::GetView(void)
{
	return bufferView_.get();
}
