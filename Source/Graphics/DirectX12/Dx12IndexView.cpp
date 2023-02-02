#include "Dx12IndexView.h"
#include <d3d12.h>
#include "../../../Include/Graphics/GpuResource.h"

Eugene::Dx12IndexView::Dx12IndexView(size_t size, Format format, GpuResource& resource)
{
    bufferView_ = std::make_unique< D3D12_INDEX_BUFFER_VIEW>();
    bufferView_->BufferLocation = static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress();
    bufferView_->Format = static_cast<DXGI_FORMAT>(format);
    bufferView_->SizeInBytes = size;
}

void* Eugene::Dx12IndexView::GetView(void) const
{
    return bufferView_.get();
}
