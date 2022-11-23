#include "Dx12UploadableResource.h"
#include <d3d12.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Common/EugeneLibException.h"

EugeneLib::Dx12UploadableResource::Dx12UploadableResource(size_t size, Graphics& graphics)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	auto device{ static_cast<ID3D12Device*>(graphics.GetDevice()) };
	if (FAILED(device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
	)))
	{
		throw EugeneLibException("アップロードリソースの作成に失敗");
	}
}

void* EugeneLib::Dx12UploadableResource::Map(void)
{
	void* ptr{ nullptr };
	resource_->Map(0, nullptr, &ptr);
	return ptr;
}

void EugeneLib::Dx12UploadableResource::UnMap(void)
{
	resource_->Unmap(0, nullptr);
}
