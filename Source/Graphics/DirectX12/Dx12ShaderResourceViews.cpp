#include "../../../Include/Graphics/DirectX12/Dx12ShaderResourceViews.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Graphics/BufferResource.h"
#include "../../../Include/Graphics/ImageResource.h"


void Eugene::ShaderResourceViews::CreateTexture(ImageResource& resource, std::uint32_t idx)
{
	if (size_ <= idx)
	{
		return;
	}

	ID3D12Resource* dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto resourceDesc = dx12Resource->GetDesc();

	auto handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};
	viewDesc.Format = resourceDesc.Format;
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	viewDesc.Texture2D.MipLevels = resourceDesc.MipLevels;

	handle.ptr += idx * Graphics::GetInstance().device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	Graphics::GetInstance().device_->CreateShaderResourceView(dx12Resource, &viewDesc, handle);
}


void Eugene::ShaderResourceViews::CreateConstantBuffer(BufferResource& resource, std::uint32_t idx)
{
	if (size_ <= idx)
	{
		return;
	}
	ID3D12Resource* dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	
	auto resourceDesc = dx12Resource->GetDesc();
	auto handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += idx * Graphics::GetInstance().device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc{};
	viewDesc.BufferLocation = dx12Resource->GetGPUVirtualAddress();
	viewDesc.SizeInBytes = static_cast<UINT>(resourceDesc.Width);
	Graphics::GetInstance().device_->CreateConstantBufferView(&viewDesc, handle);
}

void Eugene::ShaderResourceViews::CreateCubeMap(ImageResource& resource, std::uint32_t idx)
{
	if (size_ <= idx)
	{
		return;
	}
	ID3D12Resource* dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto resourceDesc = dx12Resource->GetDesc();

	auto handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};
	viewDesc.Format = resourceDesc.Format;
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	viewDesc.TextureCube.MipLevels = resourceDesc.MipLevels;

	handle.ptr += idx * Graphics::GetInstance().device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	Graphics::GetInstance().device_->CreateShaderResourceView(dx12Resource, &viewDesc, handle);
}

void Eugene::ShaderResourceViews::CreateUnorderedAccessBuffer(BufferResource& resource, std::uint32_t idx, std::uint32_t numElements, std::uint64_t strideSize)
{
	if (size_ <= idx)
	{
		return;
	}
	ID3D12Resource* dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto resourceDesc = dx12Resource->GetDesc();

	auto handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc{};
	viewDesc.Format = DXGI_FORMAT_UNKNOWN;
	viewDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	viewDesc.Buffer.NumElements = numElements;
	viewDesc.Buffer.StructureByteStride = strideSize;
	viewDesc.Buffer.FirstElement = 0;
	viewDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;


	handle.ptr += idx * Graphics::GetInstance().device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	Graphics::GetInstance().device_->CreateUnorderedAccessView(dx12Resource, nullptr, &viewDesc, handle);
}

Eugene::ShaderResourceViews& Eugene::ShaderResourceViews::operator=(const ShaderResourceViews& views)
{
	if (GetSize() > 0u)
	{
		Init(views.GetSize());
	}

	// コピーする
	Graphics::GetInstance().device_->CopyDescriptorsSimple(
		views.GetSize(),
		descriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		views.descriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);
	return *this;
}

void Eugene::ShaderResourceViews::Init(std::uint32_t size)
{
	size_ = size;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{
	D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	size,
	D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		0
	};

	if (FAILED(Graphics::GetInstance().device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("DirectX12ディスクリプタヒープの作成に失敗");
	}
}