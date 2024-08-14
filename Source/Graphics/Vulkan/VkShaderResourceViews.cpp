#include "../../../Include/Graphics/Vulkan/VkShaderResourceViews.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"
#include "../../../Include/Graphics/Vulkan/VkBufferResource.h"
#include "../../../Include/Graphics/Vulkan/VkImageResource.h"


Eugene::ShaderResourceViews::ShaderResourceViews(const ArgsSpan<Bind>& viewTypes)
{
	std::vector<vk::DescriptorSetLayoutBinding> binding(viewTypes.size());
	std::vector<vk::DescriptorPoolSize> poolSize(viewTypes.size());
	std::uint64_t num{0ull};
	for (std::uint64_t i = 0ull; i < viewTypes.size(); i++)
	{
		binding[i].setBinding(i);
		poolSize[i].setDescriptorCount(viewTypes.at(i).viewNum_);
		binding[i].setDescriptorCount(viewTypes.at(i).viewNum_);
		num += viewTypes.at(i).viewNum_;
		binding[i].setStageFlags(vk::ShaderStageFlagBits::eAll);
		switch (viewTypes.at(i).viewType_)
		{
		case ViewType::Texture:
			binding[i].setDescriptorType(vk::DescriptorType::eSampledImage);
			poolSize[i].setType(vk::DescriptorType::eSampledImage);
			break;
		case ViewType::ConstantBuffer:
			binding[i].setDescriptorType(vk::DescriptorType::eUniformBuffer);
			poolSize[i].setType(vk::DescriptorType::eUniformBuffer);
			break;
		case ViewType::UnoderedAccess:
			binding[i].setDescriptorType(vk::DescriptorType::eStorageBuffer);
			poolSize[i].setType(vk::DescriptorType::eStorageBuffer);
			break;
		case ViewType::Sampler:
			binding[i].setDescriptorType(vk::DescriptorType::eSampler);
			poolSize[i].setType(vk::DescriptorType::eSampler);
			break;
		default:
			// 特に何もしない
			break;
		}
	}
	vk::DescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.setBindingCount(1);
	layoutInfo.setBindings(binding);
	data_.layout_ = Graphics::GetInstance().device_->createDescriptorSetLayoutUnique(layoutInfo);

	vk::DescriptorPoolCreateInfo poolInfo{};
	poolInfo.setPoolSizes(poolSize);
	poolInfo.setMaxSets(1);
	poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
	descriptorPool_ = Graphics::GetInstance().device_->createDescriptorPoolUnique(poolInfo);

	vk::DescriptorSetAllocateInfo allocateInfo{};
	allocateInfo.setDescriptorPool(*descriptorPool_);
	allocateInfo.setDescriptorSetCount(1);
	allocateInfo.setSetLayouts(*data_.layout_);
	data_.descriptorSet_ = std::move(Graphics::GetInstance().device_->allocateDescriptorSetsUnique(allocateInfo)[0]);

	int idx = 0;
	typeData_.resize(num);
	for (std::uint64_t i = 0ull; i < viewTypes.size(); i++)
	{
		for (std::uint32_t j = 0ull; j < viewTypes.at(i).viewNum_; j++)
		{
			std::get<0>(typeData_[idx]) = viewTypes.at(i).viewType_;
			std::get<1>(typeData_[idx]) = i;
			std::get<2>(typeData_[idx]) = j;
			idx++;
		}
	}
}


void Eugene::ShaderResourceViews::CreateTexture(ImageResource& resource, std::uint32_t idx)
{
	if (idx >= typeData_.size())
	{
		return;
	}

	auto format = Graphics::FormatToVkFormat[static_cast<size_t>(resource.GetFormat())];
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(*resource.GetResource().image_);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(format);
	viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	viewInfo.subresourceRange.setLevelCount(resource.GetResource().mipmapLevels_);
	viewInfo.subresourceRange.setLayerCount(resource.GetResource().arraySize_);
	imageViewMap_.emplace(idx, descriptorPool_.getOwner().createImageViewUnique(viewInfo));
	auto& type = typeData_[idx];

	

	vk::DescriptorImageInfo imageInfo{};
	imageInfo.setImageView(*imageViewMap_[idx]);
	imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

	vk::WriteDescriptorSet write{};
	write.setDstSet(*data_.descriptorSet_);
	write.setDstBinding(std::get<1>(type));
	write.setDstArrayElement(std::get<2>(type));
	write.setDescriptorType(vk::DescriptorType::eSampledImage);
	write.setDescriptorCount(1);
	write.setImageInfo(imageInfo);

	data_.descriptorSet_.getOwner().updateDescriptorSets(1, &write, 0, nullptr);
}

void Eugene::ShaderResourceViews::CreateConstantBuffer(BufferResource& resource, std::uint32_t idx)
{
	if (idx >= typeData_.size())
	{
		return;
	}

	vk::DescriptorBufferInfo bufferInfo{};
	bufferInfo.setBuffer(resource.GetResource());
	bufferInfo.setOffset(0);
	bufferInfo.setRange(resource.GetSize());

	auto& type = typeData_[idx];
	vk::WriteDescriptorSet write{};
	write.setDstSet(*data_.descriptorSet_);
	write.setDstBinding(std::get<1>(type));
	write.setDstArrayElement(std::get<2>(type));
	write.setDescriptorType(vk::DescriptorType::eUniformBuffer);
	write.setDescriptorCount(1);
	write.setBufferInfo(bufferInfo);

	data_.descriptorSet_.getOwner().updateDescriptorSets(1,&write, 0, nullptr);
}

void Eugene::ShaderResourceViews::CreateCubeMap(ImageResource& resource, std::uint32_t idx)
{
	if (idx >= typeData_.size())
	{
		return;
	}


	auto format = Graphics::FormatToVkFormat[static_cast<size_t>(resource.GetFormat())];
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(*resource.GetResource().image_);
	viewInfo.setViewType(vk::ImageViewType::eCube);
	viewInfo.setFormat(format);
	viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	viewInfo.subresourceRange.setLevelCount(resource.GetResource().mipmapLevels_);
	viewInfo.subresourceRange.setLayerCount(resource.GetResource().arraySize_);
	imageViewMap_.emplace(idx, descriptorPool_.getOwner().createImageViewUnique(viewInfo));
	auto& type = typeData_[idx];

	vk::DescriptorImageInfo imageInfo{};
	imageInfo.setImageView(*imageViewMap_[idx]);
	imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

	vk::WriteDescriptorSet write{};
	write.setDstSet(*data_.descriptorSet_);
	write.setDstBinding(std::get<1>(type));
	write.setDstArrayElement(std::get<2>(type));
	write.setDescriptorType(vk::DescriptorType::eSampledImage);
	write.setDescriptorCount(1);
	write.setImageInfo(imageInfo);

	data_.descriptorSet_.getOwner().updateDescriptorSets(1, &write, 0, nullptr);
}

void Eugene::ShaderResourceViews::CreateUnorderedAccessBuffer(BufferResource& resource, std::uint32_t idx, std::uint32_t numElements, std::uint64_t strideSize)
{
	if (idx >= typeData_.size())
	{
		return;
	}

	vk::DescriptorBufferInfo bufferInfo{};
	bufferInfo.setBuffer(resource.GetResource());
	bufferInfo.setOffset(0);
	bufferInfo.setRange(resource.GetSize());

	auto& type = typeData_[idx];
	vk::WriteDescriptorSet write{};
	write.setDstSet(*data_.descriptorSet_);
	write.setDstBinding(std::get<1>(type));
	write.setDstArrayElement(std::get<2>(type));
	write.setDescriptorType(vk::DescriptorType::eStorageBuffer);
	write.setDescriptorCount(1);
	write.setBufferInfo(bufferInfo);

	data_.descriptorSet_.getOwner().updateDescriptorSets(1, &write, 0, nullptr);
}



std::uint64_t Eugene::ShaderResourceViews::GetImg(void)
{
	return std::uint64_t();
}
