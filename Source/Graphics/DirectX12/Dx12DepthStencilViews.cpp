#include "../../../Include/Utils/EugeneLibException.h"
#include "../../../Include/Graphics/DirectX12/Dx12DepthStencilViews.h"
#include "../../../Include/Graphics/DirectX12/Dx12ImageResource.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"

void Eugene::DepthStencilViews::Init(std::uint32_t size, bool isShaderVisible)
{
	size_ = size;
	isShaderVisible_ = isShaderVisible;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{
	D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
	size,
	(isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE),
		0
	};

	if (FAILED(Graphics::GetInstance().device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("DirectX12ディスクリプタヒープの作成に失敗");
	}
}

Eugene::DepthStencilViews::DepthStencilViews(const DepthStencilViews& views):
	isShaderVisible_{ views.isShaderVisible_ }
{
	*this = views;
}

Eugene::DepthStencilViews& Eugene::DepthStencilViews::operator=(const DepthStencilViews& views)
{
	if (GetSize() > 0u)
	{
		Init(views.GetSize(), views.isShaderVisible_);
	}

	// コピーする
	Graphics::GetInstance().device_->CopyDescriptorsSimple(
		views.GetSize(),
		descriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		views.descriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	);
	return *this;
}

void Eugene::DepthStencilViews::Create(ImageResource& resource, std::uint32_t idx)
{
	if (size_ <= idx)
	{
		return;
	}
	ID3D12Device* device{ nullptr };
	ID3D12Resource* dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	if (FAILED(dx12Resource->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}
	auto handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc{};
	viewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	viewDesc.Flags = D3D12_DSV_FLAG_NONE;

	if (dx12Resource->GetDesc().SampleDesc.Count > 1)
	{
		viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	}


	handle.ptr += idx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	device->CreateDepthStencilView(dx12Resource, &viewDesc, handle);
}

