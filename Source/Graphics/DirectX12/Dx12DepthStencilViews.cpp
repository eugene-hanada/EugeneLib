#include "Dx12DepthStencilViews.h"
#include "../../../Include/Graphics/ImageResource.h"

Eugene::Dx12DepthStencilViews::Dx12DepthStencilViews(ID3D12Device* device, std::uint64_t size) :
	Dx12Views{device, size, false, D3D12_DESCRIPTOR_HEAP_TYPE_DSV }, DepthStencilViews{size}
{
}

void Eugene::Dx12DepthStencilViews::Create(ImageResource& resource, std::uint64_t idx)
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
	handle.ptr += idx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	device->CreateDepthStencilView(dx12Resource, &viewDesc, handle);
}

void* Eugene::Dx12DepthStencilViews::GetViews(void) const
{
	return descriptorHeap_.Get();
}
