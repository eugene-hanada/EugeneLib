#include "Dx12SamplerViews.h"
#include <d3d12.h>
#include "Dx12Sampler.h"

Eugene::Dx12SamplerViews::Dx12SamplerViews(ID3D12Device* device, std::uint64_t size) :
	Dx12Views{ device, size,true ,D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER}, SamplerViews{}
{
}

void Eugene::Dx12SamplerViews::CreateSampler(Sampler& sampler, std::uint64_t idx)
{
	ID3D12Device* device{ nullptr };
	if (FAILED(descriptorHeap_->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}
	auto handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += idx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	device->CreateSampler(&static_cast<Dx12Sampler&>(sampler).desc_, handle);
}

void* Eugene::Dx12SamplerViews::GetViews(void)
{
	return descriptorHeap_.Get();
}
