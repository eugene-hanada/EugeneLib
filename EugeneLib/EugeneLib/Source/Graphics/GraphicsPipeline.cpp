#include "../../Include/Graphics/GraphicsPipeline.h"

#include "DirectX12/Dx12GraphicsPipeline.h"

EugeneLib::GraphicsPipeline::~GraphicsPipeline()
{
}

EugeneLib::GraphicsPipeline::GraphicsPipeline()
{
}


EugeneLib::ShaderInputLayout::ShaderInputLayout()
{
}

EugeneLib::ShaderInputLayout::ShaderInputLayout(const char* semanticName, std::uint32_t semanticIdx, Format format, std::uint32_t slot):
	semanticName_{semanticName}, semanticIdx_{semanticIdx}, format_{format}, slot_{slot}
{
}

EugeneLib::SamplerLayout::SamplerLayout(TextureAddressMode u, TextureAddressMode v, SampleFilter filter) :
	u_{u}, v_{v}, filter_{filter}
{
}
