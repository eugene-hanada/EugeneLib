#include "Dx12RenderTargetViews.h"
#include "Dx12Graphics.h"
#include "../../../Include/Graphics/ImageResource.h"

Eugene::Dx12RenderTargetViews::Dx12RenderTargetViews(ID3D12Device* device, std::uint64_t size, bool isShaderVisible) :
	Dx12Views{ device, size, isShaderVisible, D3D12_DESCRIPTOR_HEAP_TYPE_RTV },
	RenderTargetViews{size}
{
}

void Eugene::Dx12RenderTargetViews::Create(ImageResource& resource, std::uint64_t idx)
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
	const auto& desc{ dx12Resource->GetDesc() };
	rtViewDesc.Format = desc.Format;
	rtViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	if (desc.SampleDesc.Count > 1)
	{
		// サンプルカウントが1以上の場合はMSAAとする
		rtViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
	}
	handle.ptr += idx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	device->CreateRenderTargetView(static_cast<ID3D12Resource*>(resource.GetResource()), &rtViewDesc, handle);
}

void* Eugene::Dx12RenderTargetViews::GetViews(void)
{
	return descriptorHeap_.Get();
}
