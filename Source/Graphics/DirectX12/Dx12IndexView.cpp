#include "Dx12IndexView.h"
#include <d3d12.h>
#include "../../../Include/Graphics/DirectX12/Dx12BufferResource.h"
#include "../../../Include/Graphics/Graphics.h"

Eugene::Dx12IndexView::Dx12IndexView(std::uint32_t size, std::uint32_t indexNum, Format format, BufferResource& resource) :
    IndexView{indexNum}
{
    bufferView_ = std::make_unique< D3D12_INDEX_BUFFER_VIEW>();
    bufferView_->BufferLocation = static_cast<ID3D12Resource*>(resource.GetResource())->GetGPUVirtualAddress();
    auto tmp = static_cast<DXGI_FORMAT>(Graphics::FormatToDxgiFormat_.at(static_cast<int>(format)));
    bufferView_->Format = tmp;
    bufferView_->SizeInBytes = size * indexNum;
}

void* Eugene::Dx12IndexView::GetView(void)
{
    return bufferView_.get();
}
