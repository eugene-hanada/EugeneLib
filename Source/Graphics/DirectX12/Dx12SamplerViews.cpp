#include "../../../Include/Graphics/DirectX12/Dx12SamplerViews.h"
#include "../../../Include/Graphics/DirectX12/Dx12Sampler.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"


void Eugene::SamplerViews::CreateSampler(Sampler& sampler, std::uint32_t idx)
{
	if (size_ <= idx)
	{
		return;
	}
	auto handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += idx * Graphics::GetInstance().device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	Graphics::GetInstance().device_->CreateSampler(&sampler.desc_, handle);
}

void Eugene::SamplerViews::Init(std::uint32_t size)
{
	size_ = size;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{
	D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
	size,
	D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		0
	};

	if (FAILED(Graphics::GetInstance().device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("DirectX12ディスクリプタヒープの作成に失敗");
	}

}