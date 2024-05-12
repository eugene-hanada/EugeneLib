#include "Dx12ImageResource.h"
#include <dxgi1_6.h>
#include <D3D12MemAlloc.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Utils/EugeneLibException.h"
#include "Dx12Graphics.h"

Eugene::Dx12ImageResource::Dx12ImageResource(D3D12MA::Allocator* allocator, const TextureInfo& info) :
	ImageResource{info.format}
{
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(info.format)))
		, static_cast<std::uint32_t>(info.width),
		static_cast<std::uint32_t>(info.height),
		info.arraySize,
		info.mipLevels
	);

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
		throw CreateErrorException("テクスチャ用リソース生成失敗");
	}
}

Eugene::Dx12ImageResource::Dx12ImageResource(
	D3D12MA::Allocator* allocator, 
	const glm::ivec2& size,
	Format format,
	std::uint32_t arraySize,
	std::uint8_t mipLeveles,
	std::uint8_t sampleCount,
	std::optional<std::span<float, 4>> clearColor) :
	ImageResource{format}
{
	auto tmp = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(format)));
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		tmp, static_cast<std::uint64_t>(size.x), static_cast<std::uint64_t>(size.y),
		arraySize, mipLeveles,1,sampleCount
	);
	
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

	auto clear{ CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor.has_value() ? clearColor.value().data(): nullptr) };
	if (FAILED(allocator->CreateResource(
		&allocationDesc,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		clearColor.has_value() ? &clear : nullptr,
		allocation_.ReleaseAndGetAddressOf(),
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
	)))
	{
		throw CreateErrorException("レンダーターゲット用リソース生成失敗");
	}
}

Eugene::Dx12ImageResource::Dx12ImageResource(D3D12MA::Allocator* allocator, const glm::ivec2& size, Format format, float clearValue, std::uint8_t sampleCount) :
	ImageResource{format}
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto tmp = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(format)));
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(tmp, static_cast<std::uint64_t>(size.x), static_cast<std::uint64_t>(size.y),1,1);
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resourceDesc.SampleDesc.Count = sampleCount;
	CD3DX12_CLEAR_VALUE clear{ DXGI_FORMAT_D32_FLOAT, clearValue, 0 };

	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

	if (FAILED(allocator->CreateResource(
		&allocationDesc,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&clear,
		allocation_.ReleaseAndGetAddressOf(),
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
	)))
	{
		throw CreateErrorException("デプス用リソース生成失敗");
	}
}


Eugene::Dx12ImageResource::Dx12ImageResource(IDXGISwapChain4* swapChain, std::uint32_t idx) :
	ImageResource{Format::R8G8B8A8_UNORM}
{
	if (FAILED(swapChain->GetBuffer(idx, IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("スワップチェイン用リソース生成失敗");
	}
}


glm::ivec2 Eugene::Dx12ImageResource::GetSize(void)
{
	return { static_cast<int>(resource_->GetDesc().Width) , static_cast<int>(resource_->GetDesc().Height) };
}

bool Eugene::Dx12ImageResource::CanMap(void) const
{
	return false;
}

void* Eugene::Dx12ImageResource::GetResource(void) 
{
	return resource_.Get();
}
