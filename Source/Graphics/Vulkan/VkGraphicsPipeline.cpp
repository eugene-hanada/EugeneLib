#include "../../../Include/Graphics/Vulkan/VkGraphicsPipeline.h"
#include "../../../Include/Graphics/Vulkan/VkResourceBindLayout.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"
#include "../../../Include/Graphics/Shader.h"
#include "../../../Include/Math/Math.h"

void Eugene::Pipeline::Init(
	ResourceBindLayout& resourceBindLayout,
	const ArgsSpan<ShaderInputLayout>& layout, 
	const ArgsSpan<ShaderPair>& shaders,
	const ArgsSpan<RendertargetLayout>& rendertarges,
	TopologyType topologyType,
	bool isCulling,
	bool useDepth, 
	std::uint8_t sampleCount
)
{
	data_.layout_ = resourceBindLayout.pipelineLayout_;

	// シェーダーステージの設定
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStage(shaders.size());
	std::vector<vk::UniqueShaderModule> modules(shaders.size());
	int shaderIndex = 0;
	for (std::uint64_t i = 0ull; i < shaders.size(); i++)
	{
		vk::ShaderModuleCreateInfo info{};
		info.setPCode(reinterpret_cast<const std::uint32_t*>(shaders.at(i).first.GetPtr()));
		info.setCodeSize(shaders.at(i).first.GetSize());

		modules[shaderIndex] = Graphics::GetInstance().device_->createShaderModuleUnique(info);
		shaderStage[shaderIndex].setModule(*modules[shaderIndex]);
		shaderStage[shaderIndex].setPName("main");
		//shaderStage[shaderIndex].setPName("main");

		switch (shaders.at(i).second)
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

	// ダイナミックステートを設定
	constexpr vk::DynamicState dynamicStates[]{
	vk::DynamicState::eViewport,
	vk::DynamicState::eScissor,
	vk::DynamicState::ePrimitiveTopology
	};
	vk::PipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.setDynamicStates(dynamicStates);

	// 頂点入力の設定
	vk::VertexInputBindingDescription vertexInputLayout{};
	vertexInputLayout.setBinding(0);
	vertexInputLayout.setInputRate(vk::VertexInputRate::eVertex);
	std::vector<vk::VertexInputAttributeDescription> vertexInputAtr(layout.size());
	int atrIndex{ 0 };
	size_t layoutByte{ 0ull };
	for (std::uint64_t i = 0ull; i < layout.size(); i++)
	{
		auto tmpFormat = layout.at(i).format_;
		if (tmpFormat == Format::AUTO_BACKBUFFER)
		{
			tmpFormat = Graphics::BackBufferFormat();
		}
		vertexInputAtr[i].setBinding(0);
		vertexInputAtr[i].setLocation(i);
		vertexInputAtr[i].setFormat(Graphics::FormatToVkFormat[static_cast<size_t>(tmpFormat)]);
		vertexInputAtr[i].setOffset(layoutByte);
		layoutByte += FormatSize[static_cast<size_t>(layout.at(i).format_)];

	}
	vertexInputLayout.setStride(layoutByte);
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.setVertexBindingDescriptions(vertexInputLayout);
	vertexInputInfo.setVertexAttributeDescriptions(vertexInputAtr);


	// 入力アセンブリの設定
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;

	// トポロジー設定
	constexpr vk::PrimitiveTopology toVkPrimitiveTopology[]{
		vk::PrimitiveTopology::ePointList,
		vk::PrimitiveTopology::ePointList,
		vk::PrimitiveTopology::eLineList,
		vk::PrimitiveTopology::eTriangleList,
		vk::PrimitiveTopology::ePatchList
	};
	inputAssembly.setTopology(toVkPrimitiveTopology[static_cast<size_t>(topologyType)]);

	// trueで線と三角形になったり_STRIPだったりdocument見る
	inputAssembly.setPrimitiveRestartEnable(false);

	// ラスタライザの設定
	vk::PipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.setRasterizerDiscardEnable(false);
	rasterizer.setDepthBiasEnable(useDepth);
	rasterizer.setPolygonMode(vk::PolygonMode::eFill);
	rasterizer.setLineWidth(1.0f);


	if (isCulling)
	{
		rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
	}
	// 時計周りにする(DirectXと同じ)
	rasterizer.setFrontFace(vk::FrontFace::eClockwise);

	vk::PipelineMultisampleStateCreateInfo multiSampleInfo{};
	multiSampleInfo.setSampleShadingEnable(false);
	if (sampleCount > 1)
	{
		multiSampleInfo.setRasterizationSamples(static_cast<vk::SampleCountFlagBits>(sampleCount));
	}
	else
	{

		multiSampleInfo.setRasterizationSamples(vk::SampleCountFlagBits::e1);
	}

	std::vector<vk::PipelineColorBlendAttachmentState> attachments{ rendertarges.size() };
	std::vector<vk::Format> attachmentsFormat(rendertarges.size());

	for (size_t i = 0ull; i < rendertarges.size(); i++)
	{
		auto tmpFormat = rendertarges.at(i).format_;
		if (tmpFormat == Format::AUTO_BACKBUFFER)
		{
			tmpFormat = Graphics::BackBufferFormat();
		}
		attachmentsFormat[i] = Graphics::FormatToVkFormat[static_cast<size_t>(tmpFormat)];
		constexpr vk::ColorComponentFlags toComponentFlag[]{
			static_cast<const vk::ColorComponentFlags>(0u),
			// R32G32B32A32
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,

			// R32G32B32
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB,

			// R32G32
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,

			// R32
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,

			// R16G16B16A16
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,

			// R16G16
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG,

			// R16
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,
			vk::ColorComponentFlagBits::eR,

			// R8G8B8A8
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,

			// B8G8R8A8
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
		};

		attachments[i].setColorWriteMask(toComponentFlag[static_cast<size_t>(tmpFormat)]);
		switch (rendertarges.at(i).blendType_)
		{
		case BlendType::Non:
			attachments[i].setBlendEnable(false);
			break;
		case BlendType::Alpha:
			attachments[i].setBlendEnable(true);
			attachments[i].setColorBlendOp(vk::BlendOp::eAdd);
			attachments[i].setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
			attachments[i].setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
			attachments[i].setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha);
			attachments[i].setSrcAlphaBlendFactor(vk::BlendFactor::eDstAlpha);
			break;
		case BlendType::Add:
			attachments[i].setBlendEnable(true);
			attachments[i].setColorBlendOp(vk::BlendOp::eAdd);
			attachments[i].setSrcColorBlendFactor(vk::BlendFactor::eSrcColor);
			attachments[i].setDstColorBlendFactor(vk::BlendFactor::eDstColor);
			attachments[i].setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha);
			attachments[i].setSrcAlphaBlendFactor(vk::BlendFactor::eDstAlpha);
			break;
		case BlendType::Sub:
			attachments[i].setBlendEnable(true);
			attachments[i].setColorBlendOp(vk::BlendOp::eSubtract);
			attachments[i].setSrcColorBlendFactor(vk::BlendFactor::eSrcColor);
			attachments[i].setDstColorBlendFactor(vk::BlendFactor::eDstColor);
			attachments[i].setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha);
			attachments[i].setSrcAlphaBlendFactor(vk::BlendFactor::eDstAlpha);
			break;
		case BlendType::Inv:
			attachments[i].setBlendEnable(true);
			attachments[i].setColorBlendOp(vk::BlendOp::eAdd);
			attachments[i].setSrcColorBlendFactor(vk::BlendFactor::eOneMinusSrcColor);
			attachments[i].setDstColorBlendFactor(vk::BlendFactor::eDstColor);
			attachments[i].setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha);
			attachments[i].setSrcAlphaBlendFactor(vk::BlendFactor::eDstAlpha);
			break;
		default:
			// 実装しない
			break;
		}

	}

	vk::PipelineColorBlendStateCreateInfo blendInfo{};
	blendInfo.setAttachments(attachments);
	blendInfo.setLogicOpEnable(VK_FALSE);

	vk::PipelineDepthStencilStateCreateInfo depthInfo{};
	depthInfo.setDepthTestEnable(useDepth);
	depthInfo.setDepthWriteEnable(useDepth);
	depthInfo.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
	depthInfo.setMaxDepthBounds(1.0f);
	depthInfo.setMinDepthBounds(0.0f);

	vk::PipelineRenderingCreateInfo renderingInfo{};
	renderingInfo.setColorAttachmentCount(static_cast<std::uint32_t>(attachmentsFormat.size()));
	renderingInfo.setColorAttachmentFormats(attachmentsFormat);
	if (useDepth)
	{
		renderingInfo.setDepthAttachmentFormat(vk::Format::eD32Sfloat);
	}
	vk::Viewport viewport{ 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f };
	vk::Rect2D scissor;
	scissor.setOffset({ 0,0 });
	scissor.setExtent(vk::Extent2D{ 1280, 720 });

	// これでシザーとビューポート固定できる(指定しなければ動的になる)
	vk::PipelineViewportStateCreateInfo viewportState;

	// それぞれ数を指定
	viewportState.setViewportCount(1);
	viewportState.setScissorCount(1);

	// グラフィックパイプラインの設定
	vk::GraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.setStages(shaderStage);
	pipelineInfo.setPVertexInputState(&vertexInputInfo);
	pipelineInfo.setPInputAssemblyState(&inputAssembly);
	pipelineInfo.setPRasterizationState(&rasterizer);
	pipelineInfo.setPDynamicState(&dynamicState);
	pipelineInfo.setPMultisampleState(&multiSampleInfo);
	pipelineInfo.setPViewportState(&viewportState);
	pipelineInfo.setPNext(&renderingInfo);

	//pipelineInfo.setRenderPass(*tmp);
	// デプス周りとりあえずなしで実装
	if (useDepth)
	{
		pipelineInfo.setPDepthStencilState(&depthInfo);
	}
	pipelineInfo.setPColorBlendState(&blendInfo);
	pipelineInfo.setLayout(**data_.layout_);

	//pipelineInfo.setRenderPass(nullptr);

	auto [result, pipeline] = Graphics::GetInstance().device_->createGraphicsPipelineUnique(nullptr, pipelineInfo);

	if (result != vk::Result::eSuccess)
	{
		throw CreateErrorException{ "グラフィックスパイプラインステート生成失敗" };
	}

	data_.pipeline_ = std::move(pipeline);
}


void Eugene::Pipeline::Init(ResourceBindLayout& resourceBindLayout, const Shader& csShader)
{
	data_.layout_ = resourceBindLayout.pipelineLayout_;

	vk::ShaderModuleCreateInfo info{};
	info.setPCode(reinterpret_cast<const std::uint32_t*>(csShader.GetPtr()));
	info.setCodeSize(csShader.GetSize());

	auto csModule = Graphics::GetInstance().device_->createShaderModuleUnique(info);

	vk::PipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.setStage(vk::ShaderStageFlagBits::eCompute);
	shaderStageInfo.setModule(*csModule);
	shaderStageInfo.setPName("main");

	vk::ComputePipelineCreateInfo createInfo{};
	createInfo.setLayout(**data_.layout_);
	createInfo.setStage(shaderStageInfo);
	auto [result, pipeline] = Graphics::GetInstance().device_->createComputePipelineUnique(nullptr,createInfo);
	data_.pipeline_ = std::move(pipeline);
}
