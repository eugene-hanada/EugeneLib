#include "../../../Include/Graphics/DirectX12/Dx12Sampler.h"
#include <array>

Eugene::Sampler::Sampler(const SamplerLayout& layout)
{
	desc_.Filter = static_cast<D3D12_FILTER>(layout.filter_);
	desc_.AddressU = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(layout.u_);
	desc_.AddressV = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(layout.v_);
	desc_.AddressW = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(layout.w_);
	desc_.MipLODBias = 0.0f;
	desc_.MaxAnisotropy = layout.maxAnisotropy_;
	desc_.ComparisonFunc = static_cast<D3D12_COMPARISON_FUNC>(layout.comparison_);
	desc_.MaxLOD = layout.maxLod_;
	desc_.MinLOD = layout.minLod_;
	std::fill(std::begin(desc_.BorderColor),std::end(desc_.BorderColor), 1.0f);
}
