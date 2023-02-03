#include "Dx12UploadableResource.h"
#include <d3d12.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "../../../Include/Graphics/Image.h"

Eugene::Dx12UploadableResource::Dx12UploadableResource(ID3D12Device* device,size_t size)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
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

Eugene::Dx12UploadableResource::Dx12UploadableResource(ID3D12Device* device, Image& texture)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// アップロード先のdescをそうていする
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT  footprint{};
	std::uint64_t totalSize;
	std::uint64_t rowSize;
	std::uint32_t numRaw;
	auto footDesc = CD3DX12_RESOURCE_DESC::Tex2D(static_cast<DXGI_FORMAT>(texture.GetInfo().format), texture.GetInfo().width, texture.GetInfo().height);
	device->GetCopyableFootprints(&footDesc, 0, 1, 0, &footprint, &numRaw, &rowSize, &totalSize);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);
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
	

	

	std::uint8_t* ptr{ nullptr };
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
	auto texPtr = texture.GetData();
	for (std::uint32_t i = 0u; i < numRaw; i++)
	{
		std::copy_n(texPtr, rowSize, ptr);
		ptr += footprint.Footprint.RowPitch;
		texPtr += rowSize;
	}
	resource_->Unmap(0, nullptr);
}

void* Eugene::Dx12UploadableResource::Map(void)
{
	void* ptr{ nullptr };
	resource_->Map(0, nullptr, &ptr);
	return ptr;
}

void Eugene::Dx12UploadableResource::UnMap(void)
{
	resource_->Unmap(0, nullptr);
}
