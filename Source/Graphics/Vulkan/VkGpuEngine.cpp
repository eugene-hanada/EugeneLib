#include "VkGpuEngine.h"
#include "VkCommandList.h"

Eugene::VkGpuEngine::VkGpuEngine(vk::Queue& queue, std::uint64_t size) :
	queue_{queue}
{
	nowNum_ = 0ull;
	cmdBuffers_.resize(size);
}

Eugene::VkGpuEngine::VkGpuEngine(std::uint32_t familyIndex, std::uint32_t& queueIndex, const vk::Device& device, std::uint64_t size)
{
	queue_ = device.getQueue(familyIndex, queueIndex++);

	nowNum_ = 0ull;
	cmdBuffers_.resize(size);
}

Eugene::VkGpuEngine::~VkGpuEngine()
{ 
}

void Eugene::VkGpuEngine::Execute(void)
{
	vk::SubmitInfo info{};
	info.setCommandBufferCount(nowNum_);
	info.setPCommandBuffers(cmdBuffers_.data());
	queue_.submit(info);
	nowNum_ = 0;
}

void Eugene::VkGpuEngine::Wait(void)
{
	queue_.waitIdle();
}

void Eugene::VkGpuEngine::Push(CommandList& commandList)
{
	auto data = static_cast<vk::UniqueCommandBuffer*>(commandList.GetCommandList());
	cmdBuffers_[nowNum_] = **data;
	nowNum_++;
}

void* Eugene::VkGpuEngine::GetQueue(void) const
{
	return const_cast<void*>(static_cast<const void*>(&queue_));
}
