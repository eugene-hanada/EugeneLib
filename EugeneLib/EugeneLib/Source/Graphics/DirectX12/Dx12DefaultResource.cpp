#include "Dx12DefaultResource.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Common/EugeneLibException.h"

EugeneLib::Dx12DefaultResource::Dx12DefaultResource(size_t size, Graphics& graphics)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	auto device{ static_cast<ID3D12Device*>(graphics.GetDevice()) };
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

EugeneLib::Dx12DefaultResource::Dx12DefaultResource(std::uint32_t idx, Graphics& graphics)
{
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	auto swapChain{ static_cast<IDXGISwapChain4*>(graphics.GetSwapChain()) };
	if (FAILED(swapChain->GetBuffer(idx, IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException("スワップチェイン用バッファの作成に失敗");
	}

}

EugeneLib::Dx12DefaultResource::Dx12DefaultResource(const Vector2& size, Format format, const std::span<float, 4>& clearColor, Graphics& graphics)
{
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(static_cast<DXGI_FORMAT>(format), static_cast<std::uint64_t>(size.x), static_cast<std::uint64_t>(size.y));
	resourceDesc.Alignment = 65536;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resourceDesc.MipLevels = 0;
	auto clear{ CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM,clearColor.data()) };
	auto device{ static_cast<ID3D12Device*>(graphics.GetDevice()) };
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
