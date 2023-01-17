#include "Dx12DefaultResource.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Common/EugeneLibException.h"

EugeneLib::Dx12DefaultResource::Dx12DefaultResource(ID3D12Device* device, size_t size)
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

EugeneLib::Dx12DefaultResource::Dx12DefaultResource(IDXGISwapChain4* swapChain, std::uint32_t idx)
{
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	if (FAILED(swapChain->GetBuffer(idx, IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException("スワップチェイン用バッファの作成に失敗");
	}

}

EugeneLib::Dx12DefaultResource::Dx12DefaultResource(ID3D12Device* device, const Vector2& size, Format format, const std::span<float, 4>& clearColor)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(static_cast<DXGI_FORMAT>(format), static_cast<std::uint64_t>(size.x), static_cast<std::uint64_t>(size.y));
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
		throw EugeneLibException("レンダーターゲット用リソースの作成に失敗");
	}
}

EugeneLib::Dx12DefaultResource::Dx12DefaultResource(ID3D12Device* device, const TextureInfo& formatData)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		static_cast<DXGI_FORMAT>(formatData.format)
		, static_cast<std::uint32_t>(formatData.width),
		static_cast<std::uint32_t>(formatData.height)
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
		throw EugeneLibException("テクスチャリソース作成");
	}
}

EugeneLib::Dx12DefaultResource::Dx12DefaultResource(ID3D12Device* device,const Vector2& size, Format format, float clearValue)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(static_cast<DXGI_FORMAT>(format), static_cast<std::uint64_t>(size.x), static_cast<std::uint64_t>(size.y));
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
		throw EugeneLibException("デプス用リソースの作成に失敗");
	}
}
