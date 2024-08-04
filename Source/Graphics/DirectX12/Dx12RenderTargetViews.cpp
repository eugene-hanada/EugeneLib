#include "../../../Include/Graphics/DirectX12/Dx12RenderTargetViews.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include "../../../Include/Graphics/ImageResource.h"

void Eugene::RenderTargetViews::Create(ImageResource& resource, std::uint32_t idx)
{
	if (size_ <= idx)
	{
		return;
	}

	ID3D12Resource* dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	
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
	handle.ptr += idx * Graphics::GetInstance().device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	Graphics::GetInstance().device_->CreateRenderTargetView(static_cast<ID3D12Resource*>(resource.GetResource()), &rtViewDesc, handle);
}

Eugene::RenderTargetViews& Eugene::RenderTargetViews::operator=(const RenderTargetViews& views)
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
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV
	);
	return *this;
}

void Eugene::RenderTargetViews::Init(std::uint32_t size, bool isShaderVisible)
{
	size_ = size;
	isShaderVisible_ = isShaderVisible;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{
	D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
	size,
	(isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE),
		0
	};

	if (FAILED(Graphics::GetInstance().device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("DirectX12ディスクリプタヒープの作成に失敗");
	}
}