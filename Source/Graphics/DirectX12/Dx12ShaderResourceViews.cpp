#include "Dx12ShaderResourceViews.h"
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/BufferResource.h"
#include "../../../Include/Graphics/ImageResource.h"

Eugene::Dx12ShaderResourceViews::Dx12ShaderResourceViews(ID3D12Device* device, std::uint64_t size) :
	Dx12Views{device,size,true,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV}, ShaderResourceViews{size}
{
}


void Eugene::Dx12ShaderResourceViews::CreateTexture(ImageResource& resource, std::uint64_t idx)
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
	viewDesc.Texture2D.MipLevels = resourceDesc.MipLevels;

	handle.ptr += idx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	device->CreateShaderResourceView(dx12Resource, &viewDesc, handle);
}


void Eugene::Dx12ShaderResourceViews::CreateConstantBuffer(BufferResource& resource, std::uint64_t idx)
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

void Eugene::Dx12ShaderResourceViews::CreateCubeMap(ImageResource& resource, std::uint64_t idx)
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
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	viewDesc.TextureCube.MipLevels = resourceDesc.MipLevels;

	handle.ptr += idx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	device->CreateShaderResourceView(dx12Resource, &viewDesc, handle);
}

void* Eugene::Dx12ShaderResourceViews::GetViews(void) const
{
	return descriptorHeap_.Get();
}

std::uint64_t Eugene::Dx12ShaderResourceViews::GetImg(void)
{
	ID3D12Device* device{ nullptr };
	if (FAILED(descriptorHeap_->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return 0ull;
	}
	auto handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	return handle.ptr;
}
