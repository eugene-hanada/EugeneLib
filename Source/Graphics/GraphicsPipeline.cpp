#include "../../Include/Graphics/GraphicsPipeline.h"

#include "DirectX12/Dx12GraphicsPipeline.h"

Eugene::GraphicsPipeline::~GraphicsPipeline()
{
}

Eugene::GraphicsPipeline::GraphicsPipeline()
{
}


Eugene::ShaderInputLayout::ShaderInputLayout()
{
}

Eugene::ShaderInputLayout::ShaderInputLayout(const char* semanticName, std::uint32_t semanticIdx, Format format, std::uint32_t slot):
	semanticName_{semanticName}, semanticIdx_{semanticIdx}, format_{format}, slot_{slot}
{
}
