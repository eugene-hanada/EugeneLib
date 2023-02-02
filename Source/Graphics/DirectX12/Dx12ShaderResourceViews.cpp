#include "Dx12ShaderResourceViews.h"
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/GpuResource.h"

Eugene::Dx12ShaderResourceViews::Dx12ShaderResourceViews(ID3D12Device* device, size_t size) :
	Dx12Views{device,size,true,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV}, ShaderResourceViews{size}
{
}

void Eugene::Dx12ShaderResourceViews::CreateTexture(GpuResource& resource, size_t idx)
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
	auto resourceDesc = dx12Resource->GetDesc();

	auto handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};
	viewDesc.Format = resourceDesc.Format;
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	viewDesc.Texture2D.MipLevels = 1;

	handle.ptr += idx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	device->CreateShaderResourceView(dx12Resource, &viewDesc, handle);
}


void Eugene::Dx12ShaderResourceViews::CreateConstantBuffer(GpuResource& resource, size_t idx)
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
	auto resourceDesc = dx12Resource->GetDesc();
	auto handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += idx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc{};
	viewDesc.BufferLocation = dx12Resource->GetGPUVirtualAddress();
	viewDesc.SizeInBytes = static_cast<UINT>(resourceDesc.Width);
	device->CreateConstantBufferView(&viewDesc, handle);
}

void* Eugene::Dx12ShaderResourceViews::GetViews(void) const
{
	return descriptorHeap_.Get();
}
