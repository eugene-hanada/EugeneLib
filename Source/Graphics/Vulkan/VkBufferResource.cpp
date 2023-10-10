#include "VkBufferResource.h"
#include "VkGraphics.h"

Eugene::VkBufferResource::VkBufferResource(const vk::Device& device, const VkGraphics& graphics, std::uint64_t size) :
	BufferResource{}
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.setSize(size);
	bufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);
	
	buffer_ =  device.createBufferUnique(bufferInfo);
	memory_ = std::move(graphics.CreateMemory(buffer_));
}

bool Eugene::VkBufferResource::CanMap(void) const
{
	return false;
}

void* Eugene::VkBufferResource::GetResource(void)
{
	return nullptr;
}

std::uint64_t Eugene::VkBufferResource::GetSize(void)
{
	return std::uint64_t();
}
