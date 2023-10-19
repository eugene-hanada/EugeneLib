#include "VkSamplerViews.h"
#include "VkSampler.h"
#include "VkSampler.h"

Eugene::VkSamplerViews::VkSamplerViews(const vk::Device& device, const ArgsSpan<Bind>& viewTypes) :
	SamplerViews{}
{
	std::vector<vk::DescriptorSetLayoutBinding> binding(viewTypes.size());
	std::vector<vk::DescriptorPoolSize> poolSize(viewTypes.size());
	size_t num{ 0ull };
	for (std::uint64_t i = 0ull; i < viewTypes.size(); i++)
	{
		binding[i].setBinding(i);
		binding[i].setDescriptorCount(viewTypes.at(i).viewNum_);
		binding[i].setDescriptorType(vk::DescriptorType::eSampler);
		binding[i].setStageFlags(vk::ShaderStageFlagBits::eAll);
		poolSize[i].setType(vk::DescriptorType::eSampler);
		poolSize[i].setDescriptorCount(viewTypes.at(i).viewNum_);
		num += viewTypes.at(i).viewNum_;
	}

	vk::DescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.setBindingCount(1);
	layoutInfo.setBindings(binding);
	data_.layout_ = device.createDescriptorSetLayoutUnique(layoutInfo);

	vk::DescriptorPoolCreateInfo poolInfo{};
	poolInfo.setPoolSizes(poolSize);
	poolInfo.setMaxSets(1);
	poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
	descriptorPool_ = device.createDescriptorPoolUnique(poolInfo);

	vk::DescriptorSetAllocateInfo allocateInfo{};
	allocateInfo.setDescriptorPool(*descriptorPool_);
	allocateInfo.setDescriptorSetCount(1);
	allocateInfo.setSetLayouts(*data_.layout_);
	data_.descriptorSet_ = std::move(device.allocateDescriptorSetsUnique(allocateInfo)[0]);


	typeData_.resize(num);
	int idx{ 0 };
	for (std::uint64_t i = 0ull; i < viewTypes.size(); i++)
	{
		for (std::uint32_t j = 0ull; j < viewTypes.at(i).viewNum_; j++)
		{
			typeData_[idx].first = i;
			typeData_[idx].second = j;
			idx++;
		}
	}
}

void Eugene::VkSamplerViews::CreateSampler(Sampler& sampler, std::uint64_t idx)
{
	if (idx >= typeData_.size())
	{
		return;
	}

	auto& type{ typeData_[idx] };

	vk::DescriptorImageInfo  info{};
	info.setSampler(*static_cast<VkSampler&>(sampler).sampler_);

	vk::WriteDescriptorSet write;
	write.setDescriptorType(vk::DescriptorType::eSampler);
	write.setDstBinding(type.first);
	write.setDstArrayElement(type.second);
	write.setImageInfo(info);
	write.setDescriptorCount(1);
	write.setDstSet(*data_.descriptorSet_);

	data_.descriptorSet_.getOwner().updateDescriptorSets(1, &write, 0, nullptr);
}

void* Eugene::VkSamplerViews::GetViews(void)
{
	return &data_;
}
