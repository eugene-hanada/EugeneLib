#include "Dx12RenderTargetViews.h"
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Graphics/GpuResource.h"

Eugene::Dx12RenderTargetViews::Dx12RenderTargetViews(ID3D12Device* device, size_t size, bool isShaderVisible) :
	Dx12Views{ device, size, isShaderVisible, D3D12_DESCRIPTOR_HEAP_TYPE_RTV },
	RenderTargetViews{size}
{
}

void Eugene::Dx12RenderTargetViews::Create(GpuResource& resource, size_t idx, const Format& format)
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
	D3D12_RENDER_TARGET_VIEW_DESC rtViewDesc{};
	rtViewDesc.Format = static_cast<DXGI_FORMAT>(format);
	rtViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	handle.ptr += idx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	device->CreateRenderTargetView(static_cast<ID3D12Resource*>(resource.GetResource()), &rtViewDesc, handle);
}

void* Eugene::Dx12RenderTargetViews::GetViews(void) const
{
	return descriptorHeap_.Get();
}
