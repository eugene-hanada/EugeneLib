#include "Dx12SamplerVies.h"
#include <d3d12.h>
#include "Dx12Sampler.h"

Eugene::Dx12SamplerViews::Dx12SamplerViews(ID3D12Device* device, size_t size) :
	Dx12Views{ device, size,true ,D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER}, SamplerViews{}
{
}

void Eugene::Dx12SamplerViews::CreateSampler(Sampler& sampler, size_t idx)
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

void* Eugene::Dx12SamplerViews::GetViews(void) const
{
	return descriptorHeap_.Get();
}
