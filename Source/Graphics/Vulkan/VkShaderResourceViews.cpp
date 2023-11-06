#include "VkShaderResourceViews.h"
#include "VkBufferResource.h"
#include "VkImageResource.h"
#include "VkGraphics.h"

Eugene::VkShaderResourceViews::VkShaderResourceViews(const vk::Device& device, const ArgsSpan<Bind>& viewTypes) :
	ShaderResourceViews{viewTypes.size()}
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

	int idx = 0;
	typeData_.resize(num);
	size_ = num;
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

Eugene::VkShaderResourceViews::~VkShaderResourceViews()
{
	size_;
}

void Eugene::VkShaderResourceViews::CreateTexture(ImageResource& resource, std::uint64_t idx)
{
	if (idx >= size_)
	{
		return;
	}

	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource())};
	auto format = VkGraphics::FormatToVkFormat[static_cast<size_t>(resource.GetFormat())];
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(*data->image_);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(format);
	viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	viewInfo.subresourceRange.setLevelCount(data->mipmapLevels_);
	viewInfo.subresourceRange.setLayerCount(data->arraySize_);
	imageViewMap_.emplace(idx,data->image_.getOwner().createImageViewUnique(viewInfo));
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

void Eugene::VkShaderResourceViews::CreateConstantBuffer(BufferResource& resource, std::uint64_t idx)
{
	if (idx >= size_)
	{
		return;
	}

	vk::DescriptorBufferInfo bufferInfo{};
	bufferInfo.setBuffer(*static_cast<VkBufferData*>(resource.GetResource())->buffer_);
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

void Eugene::VkShaderResourceViews::CreateCubeMap(ImageResource& resource, std::uint64_t idx)
{
	if (idx >= size_)
	{
		return;
	}

	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };
	auto format = VkGraphics::FormatToVkFormat[static_cast<size_t>(resource.GetFormat())];
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(*data->image_);
	viewInfo.setViewType(vk::ImageViewType::eCube);
	viewInfo.setFormat(format);
	viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	viewInfo.subresourceRange.setLevelCount(data->mipmapLevels_);
	viewInfo.subresourceRange.setLayerCount(data->arraySize_);
	imageViewMap_.emplace(idx, data->image_.getOwner().createImageViewUnique(viewInfo));
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

void* Eugene::VkShaderResourceViews::GetViews(void)
{
	return &data_;
}

std::uint64_t Eugene::VkShaderResourceViews::GetImg(void)
{
	return std::uint64_t();
}
