#include "Dx12BufferResource.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3D12MemAlloc.h>

#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Utils/EugeneLibException.h"
#include "../../../Include/Graphics/Image.h"
#include "Dx12Graphics.h"

Eugene::Dx12BufferResource::Dx12BufferResource(D3D12MA::Allocator* allocator, std::uint64_t size) :
	BufferResource{}
{
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	
	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

	if (FAILED(allocator->CreateResource(
			&allocationDesc,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			allocation_.ReleaseAndGetAddressOf(),
			IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
		)))
	{
		throw CreateErrorException("ID3D12Resourceをデフォルトリソースで生成に失敗");
	}
}

Eugene::Dx12BufferResource::~Dx12BufferResource()
{
}

bool Eugene::Dx12BufferResource::CanMap(void) const
{
	return false;
}

void* Eugene::Dx12BufferResource::GetResource(void) 
{
	return resource_.Get();
}

std::uint64_t Eugene::Dx12BufferResource::GetSize(void)
{
	return resource_->GetDesc().Width;
}

Eugene::Dx12UploadableBufferResource::Dx12UploadableBufferResource(ID3D12Device* device, D3D12MA::Allocator* allocator, Image& image)
{
	auto subResource = std::min(std::max(static_cast<int>(image.GetInfo().mipLevels) , 1) * static_cast<int>(image.GetInfo().arraySize), static_cast<int>(maxSubResource));

	// アップロード先のdescをそうていする
	std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT,maxSubResource>  footprint;
	
	// トータルのバイト数
	std::uint64_t totalSize;

	std::array<std::uint64_t,maxSubResource> rowSize;	
	std::array<std::uint32_t,maxSubResource> numRaw;
	

	auto tmp = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(image.GetInfo().format)));
	auto footDesc = CD3DX12_RESOURCE_DESC::Tex2D(tmp, image.GetInfo().width, image.GetInfo().height, image.GetInfo().arraySize, image.GetInfo().mipLevels);
	device->GetCopyableFootprints(&footDesc, 0, subResource, 0, footprint.data(), numRaw.data(), rowSize.data(), &totalSize);

	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);
	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
	
	if (FAILED(allocator->CreateResource(
		&allocationDesc,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		allocation_.ReleaseAndGetAddressOf(),
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("ID3D12Resourceをアップロードリソースで作成に失敗");
	}

	std::uint8_t* ptr{ nullptr };
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
	
	// array分
	for (std::uint32_t j = 0ull; j < image.GetInfo().arraySize; j++)
	{
		// ミップマップ分
		for (std::uint32_t m = 0; m < image.GetInfo().mipLevels; m++)
		{
			// 画像データの先頭ポインタを取得する
			auto texPtr = image.GetData(j,m);

			// インデックスを計算
			auto idx = j * image.GetInfo().mipLevels + m;

			// rawを取得
			auto tmpNumRaw = numRaw[idx];
			for (std::uint32_t i = 0u; i < tmpNumRaw; i++)
			{
				// コピー
				std::copy_n(texPtr, rowSize[idx], ptr);
				ptr += footprint[idx].Footprint.RowPitch;
				texPtr += rowSize[idx];
			}
		}
	}
	resource_->Unmap(0, nullptr);
}

Eugene::Dx12UploadableBufferResource::Dx12UploadableBufferResource(D3D12MA::Allocator* allocator, std::uint64_t size) :
	BufferResource{}
{
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;


	if (FAILED(allocator->CreateResource(
		&allocationDesc,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		allocation_.ReleaseAndGetAddressOf(),
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
	)))
	{
		throw CreateErrorException("ID3D12Resourceをアップロードリソースで作成に失敗");
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

void* Eugene::Dx12UploadableBufferResource::GetResource(void) 
{
	return resource_.Get();
}

std::uint64_t Eugene::Dx12UploadableBufferResource::GetSize(void)
{
	return resource_->GetDesc().Width;
}
