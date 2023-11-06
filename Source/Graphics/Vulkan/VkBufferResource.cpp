#include "VkBufferResource.h"
#include "VkGraphics.h"
#include "../../../Include/Graphics/Image.h"

Eugene::VkBufferResource::VkBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size) :
	BufferResource{}
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.setSize(size);
	bufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);
	
	data_.buffer_ =  device.createBufferUnique(bufferInfo);
	data_.memory_ = std::move(graphics.CreateMemory(*data_.buffer_));

	device.bindBufferMemory(*data_.buffer_, *data_.memory_, 0);
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
	bufferInfo.setUsage(
		vk::BufferUsageFlagBits::eUniformBuffer | 
		vk::BufferUsageFlagBits::eTransferSrc | 
		vk::BufferUsageFlagBits::eTransferDst | 
		vk::BufferUsageFlagBits::eVertexBuffer
	);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

	data_.buffer_ = device.createBufferUnique(bufferInfo);
	data_.memory_ = std::move(graphics.CreateMemory(*data_.buffer_, false, true));

	device.bindBufferMemory(*data_.buffer_, *data_.memory_, 0);
}

Eugene::VkUploadableBufferResource::VkUploadableBufferResource(const vk::Device& device, const VkGraphics& graphics, Image& image):
	BufferResource{}
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.setSize(image.GetInfo().totalSize_);
	bufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

	data_.buffer_ = device.createBufferUnique(bufferInfo);
	data_.memory_ = std::move(graphics.CreateMemory(*data_.buffer_, false, true));

	device.bindBufferMemory(*data_.buffer_, *data_.memory_, 0);

	auto ptr = static_cast<std::uint8_t*>(Map());
	std::memcpy(ptr, image.GetData(), static_cast<size_t>(image.GetInfo().totalSize_));
	UnMap();
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
