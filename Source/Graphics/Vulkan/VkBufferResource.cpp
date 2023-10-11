#include "VkBufferResource.h"
#include "VkGraphics.h"

Eugene::VkBufferResource::VkBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size) :
	BufferResource{}
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.setSize(size);
	bufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);
	
	data_.buffer_ =  device.createBufferUnique(bufferInfo);
	data_.memory_ = std::move(graphics.CreateMemory(*data_.buffer_));
}

bool Eugene::VkBufferResource::CanMap(void) const
{
	return false;
}

void* Eugene::VkBufferResource::GetResource(void)
{
	return &data_;
}

std::uint64_t Eugene::VkBufferResource::GetSize(void)
{
	return data_.memory_.getOwner().getBufferMemoryRequirements(*data_.buffer_).size;
}

Eugene::VkUploadableBufferResource::VkUploadableBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size) :
	BufferResource{}
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.setSize(size);
	bufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

	data_.buffer_ = device.createBufferUnique(bufferInfo);
	data_.memory_ = std::move(graphics.CreateMemory(*data_.buffer_, false, true));
}

bool Eugene::VkUploadableBufferResource::CanMap(void) const
{
	return true;
}

void* Eugene::VkUploadableBufferResource::GetResource(void)
{
	return &data_;
}

std::uint64_t Eugene::VkUploadableBufferResource::GetSize(void)
{
	return data_.buffer_.getOwner().getBufferMemoryRequirements(*data_.buffer_).size;
}

void* Eugene::VkUploadableBufferResource::Map(void)
{
	return data_.buffer_.getOwner().mapMemory(*data_.memory_, 0, GetSize());
}

void Eugene::VkUploadableBufferResource::UnMap(void)
{
	data_.buffer_.getOwner().unmapMemory(*data_.memory_);
}
