#include "Dx12ImageResource.h"
#include <dxgi1_6.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "Dx12Graphics.h"

Eugene::Dx12ImageResource::Dx12ImageResource(ID3D12Device* device, const TextureInfo& info)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(info.format)))
		, static_cast<std::uint32_t>(info.width),
		static_cast<std::uint32_t>(info.height)
	);
	if (FAILED(device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
	)))
	{
		throw EugeneLibException("テクスチャ用リソース生成失敗");
	}
}

Eugene::Dx12ImageResource::Dx12ImageResource(ID3D12Device* device, const Vector2I& size, Format format, std::span<float, 4> clearColor)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto tmp = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(format)));
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(tmp, static_cast<std::uint64_t>(size.x), static_cast<std::uint64_t>(size.y));
	resourceDesc.Alignment = 65536;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resourceDesc.MipLevels = 0;
	auto clear{ CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM,clearColor.data()) };
	if (FAILED(device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&clear,
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
	)))
	{
		throw EugeneLibException("レンダーターゲット用リソース生成失敗");
	}
}

Eugene::Dx12ImageResource::Dx12ImageResource(ID3D12Device* device, const Vector2I& size, Format format, float clearValue)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto tmp = static_cast<DXGI_FORMAT>(Dx12Graphics::FormatToDxgiFormat_.at(static_cast<int>(format)));
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(tmp, static_cast<std::uint64_t>(size.x), static_cast<std::uint64_t>(size.y));
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	CD3DX12_CLEAR_VALUE clear{ DXGI_FORMAT_D32_FLOAT, clearValue, 0 };
	if (FAILED(device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clear,
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
	)))
	{
		throw EugeneLibException("デプス用リソース生成失敗");
	}
}


Eugene::Dx12ImageResource::Dx12ImageResource(IDXGISwapChain4* swapChain, std::uint32_t idx)
{
	if (FAILED(swapChain->GetBuffer(idx, IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException("スワップチェイン用リソース生成失敗");
	}
}

Eugene::Format Eugene::Dx12ImageResource::GetFormat(void) const
{
	return static_cast<Format>(resource_->GetDesc().Format);
}

Eugene::Vector2I Eugene::Dx12ImageResource::GetSize(void)
{
	return { static_cast<int>(resource_->GetDesc().Width) , static_cast<int>(resource_->GetDesc().Height) };
}

bool Eugene::Dx12ImageResource::CanMap(void) const
{
	return false;
}

void* Eugene::Dx12ImageResource::GetResource(void) const
{
	return resource_.Get();
}
