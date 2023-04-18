#include "VkGpuEngine.h"
#include "../../../Include/Graphics/CommandList.h"

Eugene::VkGpuEngine::VkGpuEngine(vk::Queue& queue, std::shared_ptr<vk::UniqueFence>& fence, std::shared_ptr<vk::UniqueSemaphore>& semaphore, std::uint64_t size) :
	queue_{queue}, fence_{fence}, semaphore_{semaphore}
{
	nowSize_ = 0ull;
	cmdBuffers_.resize(size);
}

Eugene::VkGpuEngine::VkGpuEngine(std::uint32_t familyIndex, std::uint32_t& queueIndex, const vk::Device& device, std::uint64_t size)
{
	queue_ = device.getQueue(familyIndex, queueIndex++);

	nowSize_ = 0ull;
	cmdBuffers_.resize(size);

	fence_ = std::make_shared<vk::UniqueFence>();
	vk::FenceCreateInfo fenceInfo{};
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
	*fence_ = device.createFenceUnique(fenceInfo);

	semaphore_ = std::make_shared<vk::UniqueSemaphore>();
	vk::SemaphoreCreateInfo sempInfo{};
	*semaphore_ = device.createSemaphoreUnique(sempInfo);

}

Eugene::VkGpuEngine::~VkGpuEngine()
{ 
}

void Eugene::VkGpuEngine::Execute(void)
{
	vk::SubmitInfo info{};
	info.setCommandBufferCount(nowSize_);
	info.setPCommandBuffers(cmdBuffers_.data());
	vk::Semaphore waitSemaphores[]{ **semaphore_ };
	info.setWaitSemaphores(waitSemaphores);
	vk::PipelineStageFlags waitStages[]{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
	info.setWaitDstStageMask(waitStages);
	queue_.submit(info);
	nowSize_ = 0;
}

void Eugene::VkGpuEngine::Wait(void)
{
	queue_.waitIdle();
}

void Eugene::VkGpuEngine::Push(CommandList& commandList)
{
	cmdBuffers_[nowSize_] = *reinterpret_cast<vk::CommandBuffer*>(commandList.GetCommandList());
	nowSize_++;
}

void* Eugene::VkGpuEngine::GetQueue(void) const
{
	return const_cast<void*>(static_cast<const void*>(&queue_));
}
