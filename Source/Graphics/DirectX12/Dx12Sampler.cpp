#include "Dx12Sampler.h"

Eugene::Dx12Sampler::Dx12Sampler(const SamplerLayout& layout)
{
	desc_.Filter = static_cast<D3D12_FILTER>(layout.filter_);
	desc_.AddressU = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(layout.u_);
	desc_.AddressV = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(layout.v_);
	desc_.AddressW = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(layout.w_);
}
