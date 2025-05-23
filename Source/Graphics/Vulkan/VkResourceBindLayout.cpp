#include "../../../Include/Graphics/Vulkan/VkResourceBindLayout.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"

Eugene::ResourceBindLayout::ResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes)
{
	vk::DescriptorSetLayoutCreateInfo layoutInfo{};
	std::vector<std::vector<vk::DescriptorSetLayoutBinding>> bindingVector(viewTypes.size());
	std::vector<vk::PushConstantRange> pushConstantRangeVector;
	for (std::size_t i = 0ull; i < bindingVector.size(); i++)
	{
		auto nowI = (i + viewTypes.begin());
		bindingVector[i].resize(nowI->size());
		for (std::size_t j = 0ull; j < nowI->size(); j++)
		{
			auto nowJ = nowI->begin() + j;
			if (nowJ->viewType_ == ViewType::Constans)
			{
				bindingVector[i].clear();
				pushConstantRangeVector.emplace_back(
					vk::PushConstantRange
					{
						vk::ShaderStageFlagBits::eAll,
						0,
						static_cast<std::uint32_t>(nowJ->viewNum_ * sizeof(float))
					}
				);
				break;
			}
			bindingVector[i][j].setBinding(j);
			bindingVector[i][j].setDescriptorCount(nowJ->viewNum_);
			bindingVector[i][j].setStageFlags(vk::ShaderStageFlagBits::eAll);
			switch (nowJ->viewType_)
			{
			case ViewType::Texture:
				bindingVector[i][j].setDescriptorType(vk::DescriptorType::eSampledImage);
				break;
			case ViewType::ConstantBuffer:
				bindingVector[i][j].setDescriptorType(vk::DescriptorType::eUniformBuffer);
				break;
			case ViewType::UnoderedAccess:
				bindingVector[i][j].setDescriptorType(vk::DescriptorType::eStorageBuffer);
				break;
			case ViewType::Sampler:
				bindingVector[i][j].setDescriptorType(vk::DescriptorType::eSampler);
				break;
			default:
				break;
			}
		}
		if (bindingVector[i].empty())
		{
			continue;
		}
		vk::DescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.setBindings(bindingVector[i]);
		descriptorLayoutArray_.emplace_back(Graphics::GetInstance().device_->createDescriptorSetLayout(layoutInfo));
	}
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.setSetLayouts(descriptorLayoutArray_);
	pipelineLayoutInfo.setPushConstantRanges(pushConstantRangeVector);
	pushConstantSize_ = static_cast<std::uint32_t>(pushConstantRangeVector.size());
	pipelineLayout_ = std::make_shared<vk::UniquePipelineLayout>();
	*pipelineLayout_ = Graphics::GetInstance().device_->createPipelineLayoutUnique(pipelineLayoutInfo);
}

