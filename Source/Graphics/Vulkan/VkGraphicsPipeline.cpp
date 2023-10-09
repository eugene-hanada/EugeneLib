#include "VkGraphicsPipeline.h"
#include "VkResourceBindLayout.h"
#include "../Shader.cpp"
#include "VkGraphics.h"

Eugene::VkGraphicsPipeline::VkGraphicsPipeline(
	const vk::Device& device,
	ResourceBindLayout& resourceBindLayout, 
	const ArgsSpan<ShaderInputLayout>& layout, 
	const ArgsSpan<ShaderPair>& shaders,
	const ArgsSpan<RendertargetLayout>& rendertarges,
	TopologyType topologyType,
	bool isCulling,
	bool useDepth
)
{
	auto& bindLayout{ static_cast<VkResourceBindLayout&>(resourceBindLayout) };
	data_.layout_ = bindLayout.pipelineLayout_;
	
	// シェーダーステージの用意
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStage(shaders.size());
	int shaderIndex = 0;
	for (auto b = shaders.begin(); b != nullptr; b += 1)
	{
		vk::ShaderModuleCreateInfo info{};
		info.setPCode(reinterpret_cast<const std::uint32_t*>(b->first.GetPtr()));
		info.setCodeSize(b->first.GetSize() / 4ull);
		shaderStage[shaderIndex].setModule(device.createShaderModule(info));
		switch (b->second)
		{
		case ShaderType::Vertex:
			shaderStage[shaderIndex].setStage(vk::ShaderStageFlagBits::eVertex);
			break;
		case ShaderType::Pixel:
			shaderStage[shaderIndex].setStage(vk::ShaderStageFlagBits::eFragment);
			break;
		case ShaderType::Geometry:
			shaderStage[shaderIndex].setStage(vk::ShaderStageFlagBits::eGeometry);
			break;
		case ShaderType::Domain:
			shaderStage[shaderIndex].setStage(vk::ShaderStageFlagBits::eTessellationControl);
			break;
		case ShaderType::Hull:
			shaderStage[shaderIndex].setStage(vk::ShaderStageFlagBits::eTessellationEvaluation);
			break;
		default:
			break;
		} 
		shaderIndex++;
	}

	// ダイナミックステートを用意
	constexpr vk::DynamicState dynamicStates[]{
	vk::DynamicState::eViewport,
	vk::DynamicState::eScissor
	};
	vk::PipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.setDynamicStates(dynamicStates);

	vk::VertexInputBindingDescription vertexInputLayout{};
	vertexInputLayout.setBinding(0);
	vertexInputLayout.setInputRate(vk::VertexInputRate::eVertex);
	std::vector<vk::VertexInputAttributeDescription> vertexInputAtr(layout.size());
	
	int atrIndex{ 0 };
	size_t layoutByte{ 0ull };
	for (auto b = layout.begin(); b != nullptr; b += 1)
	{
		vertexInputAtr[atrIndex].setBinding(0);
		vertexInputAtr[atrIndex].setLocation(atrIndex);
		vertexInputAtr[atrIndex].setFormat(VkGraphics::FormatToVkFormat[static_cast<size_t>(b->format_)]);
		vertexInputAtr[atrIndex].setOffset(layoutByte);
		layoutByte += FormatSize[static_cast<size_t>(b->format_)];
		
	}
	vertexInputLayout.setStride(layoutByte);

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.setVertexBindingDescriptions(vertexInputLayout);
	vertexInputInfo.setVertexAttributeDescriptions(vertexInputAtr);

}

void* Eugene::VkGraphicsPipeline::GetPipeline(void)
{
	return nullptr;
}
