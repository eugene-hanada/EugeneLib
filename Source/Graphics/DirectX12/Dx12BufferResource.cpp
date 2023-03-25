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
	auto subResource = std::min(std::max(static_cast<int>(image.GetInfo().mipLevels) , 1) * static_cast<int>(image.GetInfo().arraySize), static_cast<int>(maxSubResource));
	// アップロード先のdescをそうていする
	std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT,maxSubResource>  footprint;
	
	std::uint64_t totalSize;

	std::array<std::uint64_t,maxSubResource> rowSize;
	
	std::array<std::uint32_t,maxSubResource> numRaw;
	

	auto tmp = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(image.GetInfo().format)));
	auto footDesc = CD3DX12_RESOURCE_DESC::Tex2D(tmp, image.GetInfo().width, image.GetInfo().height, image.GetInfo().arraySize, image.GetInfo().mipLevels);
	device->GetCopyableFootprints(&footDesc, 0, subResource, 0, footprint.data(), numRaw.data(), rowSize.data(), &totalSize);
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
