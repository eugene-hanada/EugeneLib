#include "../../Include/Graphics/GraphicsPipeline.h"

EugeneLib::GraphicsPipeline::~GraphicsPipeline()
{
}

EugeneLib::GraphicsPipeline::GraphicsPipeline()
{
}


EugeneLib::ShaderInputLayout::ShaderInputLayout(const char* semanticName, std::uint32_t semanticIdx, Format format, std::uint32_t slot):
	semanticName_{semanticName}, semanticIdx_{semanticIdx}, format_{format}, slot_{slot}
{
}
