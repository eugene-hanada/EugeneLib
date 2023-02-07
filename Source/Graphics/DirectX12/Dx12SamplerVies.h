#pragma once
#include "Dx12Views.h"
#include "../../../Include/Graphics/SamplerViews.h"

namespace Eugene
{
    class Sampler;
    class Dx12SamplerViews :
        public Dx12Views,  public SamplerViews
    {
    public:
        Dx12SamplerViews(ID3D12Device* device, size_t size);
    private:
        void CreateSampler(Sampler& sampler, size_t idx) final;
        void* GetViews(void) const final;
    };
}
