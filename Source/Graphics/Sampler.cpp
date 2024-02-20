#include "../../Include/Graphics/Sampler.h"
#include <limits>



Eugene::SamplerLayout::SamplerLayout(TextureAddressMode u, TextureAddressMode v, TextureAddressMode w, SampleFilter filter) :
	filter_{filter}, u_{u}, v_{v}, w_{w}
{
	maxAnisotropy_ = 16;
	comparison_ = SamplerComparison::Non;
	minLod_ = 0.0f;
	
	maxLod_ = std::numeric_limits<float>::max();
}

Eugene::Sampler::~Sampler()
{
}


Eugene::Sampler::Sampler()
{
}
