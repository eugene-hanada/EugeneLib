#include "Dx12BufferResource.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "../../../Include/Graphics/Image.h"
#include "Dx12Graphics.h"

Eugene::Dx12BufferResource::Dx12BufferResource(ID3D12Device* device, std::uint64_t size) :
	BufferResource{}
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	if (FAILED(device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
	)))
	{
		throw EugeneLibException("デフォルトリソースの作成に失敗");
	}
}

Eugene::Dx12BufferResource::~Dx12BufferResource()
{
}

bool Eugene::Dx12BufferResource::CanMap(void) const
{
	return false;
}

void* Eugene::Dx12BufferResource::GetResource(void) const
{
	return resource_.Get();
}

std::uint64_t Eugene::Dx12BufferResource::GetSize(void)
{
	return resource_->GetDesc().Width;
}

Eugene::Dx12UploadableBufferResource::Dx12UploadableBufferResource(ID3D12Device* device, Image& image)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// アップロード先のdescをそうていする
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT  footprint{};
	std::uint64_t totalSize;
	std::uint64_t rowSize;
	std::uint32_t numRaw;
	auto tmp = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(image.GetInfo().format)));
	auto footDesc = CD3DX12_RESOURCE_DESC::Tex2D(tmp, image.GetInfo().width, image.GetInfo().height);
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
	auto texPtr = image.GetData();
	for (std::uint32_t i = 0u; i < numRaw; i++)
	{
		std::copy_n(texPtr, rowSize, ptr);
		ptr += footprint.Footprint.RowPitch;
		texPtr += rowSize;
	}
	resource_->Unmap(0, nullptr);
}

Eugene::Dx12UploadableBufferResource::Dx12UploadableBufferResource(ID3D12Device* device, std::uint64_t size) :
	BufferResource{}
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

Eugene::Dx12UploadableBufferResource::~Dx12UploadableBufferResource()
{
}

void* Eugene::Dx12UploadableBufferResource::Map(void)
{
	void* ptr{ nullptr };
	resource_->Map(0,nullptr, &ptr);
    return ptr;
}

void Eugene::Dx12UploadableBufferResource::UnMap(void)
{
	resource_->Unmap(0, nullptr);
}

bool Eugene::Dx12UploadableBufferResource::CanMap(void) const
{
	return true;
}

void* Eugene::Dx12UploadableBufferResource::GetResource(void) const
{
	return resource_.Get();
}

std::uint64_t Eugene::Dx12UploadableBufferResource::GetSize(void)
{
	return resource_->GetDesc().Width;
}
