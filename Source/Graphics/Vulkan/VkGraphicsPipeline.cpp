#include "../../../Include/Graphics/Vulkan/VkGraphicsPipeline.h"
#include "../../../Include/Graphics/Vulkan/VkResourceBindLayout.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"
#include "../../../Include/Graphics/Shader.h"
#include "../../../Include/Math/Math.h"

Eugene::Pipeline::Pipeline(
	ResourceBindLayout& resourceBindLayout,
	const ArgsSpan<ShaderInputLayout>& layout, 
	const ArgsSpan<ShaderPair>& shaders,
	const ArgsSpan<RendertargetLayout>& rendertarges,
	TopologyType topologyType,
	bool isCulling,
	const DepthStencilLayout& depthStencil,
	std::uint8_t sampleCount
)
{
	data_.layout_ = resourceBindLayout.pipelineLayout_;
	data_.pushConstantSize_ = resourceBindLayout.pushConstantSize_;
	// シェーダーステージの設定
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStage(shaders.size());
	std::vector<vk::UniqueShaderModule> modules(shaders.size());
	int shaderIndex = 0;
	for (std::uint64_t i = 0ull; i < shaders.size(); i++)
	{
		vk::ShaderModuleCreateInfo info{};
		info.setPCode(reinterpret_cast<const std::uint32_t*>(shaders.at(i).first.data()));
		info.setCodeSize(shaders.at(i).first.size());

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
	rasterizer.setDepthBiasEnable(depthStencil.depthState.enable);
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
	//{
	//	Zero,
	//		One,

	//		SrcColor,
	//		InvSrcColor,

	//		SrcAlpha,
	//		InvSrcAlpha,

	//		DstColor,
	//		InvDstColor,

	//		DstAlpha,
	//		InvDstAlpha,
	//};
	constexpr vk::BlendOp toVkBlendOp[]{
		vk::BlendOp::eAdd,
		vk::BlendOp::eSubtract,
		vk::BlendOp::eReverseSubtract,
		vk::BlendOp::eMin,
		vk::BlendOp::eMax
	};

	constexpr vk::BlendFactor toVkBlendFactor[]{
		vk::BlendFactor::eZero,
		vk::BlendFactor::eOne,
		vk::BlendFactor::eSrcColor,
		vk::BlendFactor::eOneMinusSrcColor,
		vk::BlendFactor::eSrcAlpha,
		vk::BlendFactor::eOneMinusSrcAlpha,
		vk::BlendFactor::eDstColor,
		vk::BlendFactor::eOneMinusDstColor,
		vk::BlendFactor::eDstAlpha,
		vk::BlendFactor::eOneMinusDstAlpha
	};

	for (size_t i = 0ull; i < rendertarges.size(); i++)
	{
		auto tmpFormat = rendertarges.at(i).format;
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

		auto state = rendertarges.at(i).blendState;
		attachments[i].setColorWriteMask(toComponentFlag[static_cast<size_t>(tmpFormat)]);
		
		attachments[i].setBlendEnable(rendertarges.at(i).blendState.enable);
		attachments[i].setColorBlendOp(toVkBlendOp[static_cast<size_t>(rendertarges.at(i).blendState.colorOp)]);
		attachments[i].setSrcColorBlendFactor(toVkBlendFactor[static_cast<size_t>(rendertarges.at(i).blendState.srcColor)]);
		attachments[i].setDstColorBlendFactor(toVkBlendFactor[static_cast<size_t>(rendertarges.at(i).blendState.dstColor)]);
		attachments[i].setSrcAlphaBlendFactor(toVkBlendFactor[static_cast<size_t>(rendertarges.at(i).blendState.srcAlpha)]);
		attachments[i].setDstAlphaBlendFactor(toVkBlendFactor[static_cast<size_t>(rendertarges.at(i).blendState.dstAlpha)]);
		attachments[i].setAlphaBlendOp(toVkBlendOp[static_cast<size_t>(rendertarges.at(i).blendState.alphaOp)]);
	}

	vk::PipelineColorBlendStateCreateInfo blendInfo{};
	blendInfo.setAttachments(attachments);
	blendInfo.setLogicOpEnable(VK_FALSE);

	constexpr vk::CompareOp toVkCompareOp[]{
		vk::CompareOp::eNever,
		vk::CompareOp::eLess,
		vk::CompareOp::eEqual,
		vk::CompareOp::eLessOrEqual,
		vk::CompareOp::eGreater,
		vk::CompareOp::eNotEqual,
		vk::CompareOp::eGreaterOrEqual,
		vk::CompareOp::eAlways
	};

	constexpr vk::StencilOp toVkStencilOp[]{
		vk::StencilOp::eKeep,
		vk::StencilOp::eZero,
		vk::StencilOp::eReplace,
		vk::StencilOp::eIncrementAndClamp,
		vk::StencilOp::eDecrementAndClamp,
		vk::StencilOp::eInvert,
		vk::StencilOp::eIncrementAndWrap,
		vk::StencilOp::eDecrementAndWrap
	};

	vk::PipelineDepthStencilStateCreateInfo depthInfo{};
	depthInfo.setDepthTestEnable(depthStencil.depthState.enable);
	depthInfo.setDepthWriteEnable(depthStencil.depthState.writeEnable);
	depthInfo.setDepthCompareOp(toVkCompareOp[static_cast<size_t>(depthStencil.depthState.func)]);
	depthInfo.setMaxDepthBounds(1.0f);
	depthInfo.setMinDepthBounds(0.0f);

	depthInfo.setStencilTestEnable(depthStencil.stencilState.enable);
	depthInfo.setDepthBoundsTestEnable(false);
	{
		vk::StencilOpState stencilState{};
		stencilState.setCompareMask(depthStencil.stencilState.readMask);
		stencilState.setWriteMask(depthStencil.stencilState.writeMask);
		stencilState.setDepthFailOp(toVkStencilOp[static_cast<size_t>(depthStencil.stencilState.frontState.depthFailOp)]);
		stencilState.setCompareOp(toVkCompareOp[static_cast<size_t>(depthStencil.stencilState.frontState.func)]);
		stencilState.setFailOp(toVkStencilOp[static_cast<size_t>(depthStencil.stencilState.frontState.failOp)]);
		stencilState.setPassOp(toVkStencilOp[static_cast<size_t>(depthStencil.stencilState.frontState.passOp)]);
		depthInfo.setBack(stencilState);

		stencilState.setDepthFailOp(toVkStencilOp[static_cast<size_t>(depthStencil.stencilState.backState.depthFailOp)]);
		stencilState.setCompareOp(toVkCompareOp[static_cast<size_t>(depthStencil.stencilState.backState.func)]);
		stencilState.setFailOp(toVkStencilOp[static_cast<size_t>(depthStencil.stencilState.backState.failOp)]);
		stencilState.setPassOp(toVkStencilOp[static_cast<size_t>(depthStencil.stencilState.backState.passOp)]);	
		depthInfo.setFront(stencilState);
	}
	
	vk::PipelineRenderingCreateInfo renderingInfo{};
	renderingInfo.setColorAttachmentCount(static_cast<std::uint32_t>(attachmentsFormat.size()));
	renderingInfo.setColorAttachmentFormats(attachmentsFormat);
	renderingInfo.setDepthAttachmentFormat(Graphics::FormatToVkFormat[static_cast<size_t>(depthStencil.format)]);
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

	pipelineInfo.setPDepthStencilState(&depthInfo);
	pipelineInfo.setPColorBlendState(&blendInfo);
	pipelineInfo.setLayout(**data_.layout_);
	pipelineInfo.setRenderPass(nullptr);

	auto [result, pipeline] = Graphics::GetInstance().device_->createGraphicsPipelineUnique(nullptr, pipelineInfo);

	EUGENE_ASSERT_MSG(result == vk::Result::eSuccess, "グラフィックスパイプラインステート生成失敗");

	data_.pipeline_ = std::move(pipeline);
}


Eugene::Pipeline::Pipeline(ResourceBindLayout& resourceBindLayout, std::span<std::uint8_t> csShader)
{
	data_.layout_ = resourceBindLayout.pipelineLayout_;

	vk::ShaderModuleCreateInfo info{};
	info.setPCode(reinterpret_cast<const std::uint32_t*>(csShader.data()));
	info.setCodeSize(csShader.size());

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
