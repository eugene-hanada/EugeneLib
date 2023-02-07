#pragma once
#include "../../../Include/Graphics/Sampler.h"
#include <d3d12.h>


namespace Eugene
{
    class Dx12Sampler :
        public Sampler
    {
    public:
        Dx12Sampler(const SamplerLayout& layout);
    private:
        D3D12_SAMPLER_DESC desc_;
        friend class Dx12ShaderResourceViews;
    };
}
