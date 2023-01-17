#include "Dx12Views.h"
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Common/EugeneLibException.h"

EugeneLib::Dx12Views::~Dx12Views()
{
}

EugeneLib::Dx12Views::Dx12Views(ID3D12Device* device, size_t size, bool isShaderVisible, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{ type, static_cast<std::uint32_t>(size),(isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE), 0 };
	ThrowFalse(FAILED(device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap_.ReleaseAndGetAddressOf()))),"DirectX12ディスクリプタヒープの作成に失敗");
}
