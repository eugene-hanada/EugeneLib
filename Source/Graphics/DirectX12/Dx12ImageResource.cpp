#include "../../../Include/Graphics/DirectX12/Dx12ImageResource.h"
#include <dxgi1_6.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Utils/EugeneLibException.h"
#include "../../../Include/Graphics/Graphics.h"

Eugene::ImageResource::ImageResource(const TextureInfo& info) :
	format_{info.format}
{
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		static_cast<DXGI_FORMAT>(Graphics::FormatToDxgiFormat_.at(static_cast<int>(info.format)))
		, static_cast<std::uint32_t>(info.width),
		static_cast<std::uint32_t>(info.height),
		info.arraySize,
		info.mipLevels
	);

	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

	if (FAILED(Graphics::GetInstance().allocator_->CreateResource(
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

Eugene::ImageResource::ImageResource(
	const glm::ivec2& size,
	Format format,
	std::uint32_t arraySize,
	std::uint8_t mipLeveles,
	std::uint8_t sampleCount,
	std::optional<std::span<float, 4>> clearColor) :
	format_{format}
{
	auto tmp = static_cast<DXGI_FORMAT>(Graphics::FormatToDxgiFormat_.at(static_cast<int>(format)));
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		tmp, static_cast<std::uint64_t>(size.x), static_cast<std::uint64_t>(size.y),
		arraySize, mipLeveles,sampleCount
	);
	
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
	
	D3D12_CLEAR_VALUE clear{};
	if (clearColor.has_value())
	{
		clear = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor.value().data());
	}
	if (FAILED(Graphics::GetInstance().allocator_->CreateResource(
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

Eugene::ImageResource::ImageResource(const glm::ivec2& size, Format format, float clearValue, std::uint8_t sampleCount) :
	format_{format}
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto tmp = static_cast<DXGI_FORMAT>(Graphics::FormatToDxgiFormat_.at(static_cast<int>(format)));
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(tmp, static_cast<std::uint64_t>(size.x), static_cast<std::uint64_t>(size.y),1,1);
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resourceDesc.SampleDesc.Count = sampleCount;
	CD3DX12_CLEAR_VALUE clear{ DXGI_FORMAT_D32_FLOAT, clearValue, 0 };

	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

	if (FAILED(Graphics::GetInstance().allocator_->CreateResource(
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


Eugene::ImageResource::ImageResource(std::uint32_t idx) :
	format_{Format::R8G8B8A8_UNORM}
{
	if (FAILED(Graphics::GetInstance().swapChain_->GetBuffer(idx, IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("スワップチェイン用リソース生成失敗");
	}
}

