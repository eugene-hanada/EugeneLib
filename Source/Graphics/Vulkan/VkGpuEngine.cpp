#include "VkGpuEngine.h"
#include "VkCommandList.h"

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

	/*if (semaphores_.size() > 0ull)
	{
		info.setWaitSemaphores(semaphores_);
	}*/
	/*vk::PipelineStageFlags waitStages[]{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
	info.setWaitDstStageMask(waitStages);*/
	/*vk::Semaphore waitSemaphores[]{ **semaphore_ };
	info.setSignalSemaphores(waitSemaphores);*/
	//fence_->getOwner().resetFences(**fence_);
	queue_.submit(info);
	nowSize_ = 0;
}

void Eugene::VkGpuEngine::Wait(void)
{
	queue_.waitIdle();
	//fence_->getOwner().resetFences(**fence_);
}

void Eugene::VkGpuEngine::Push(CommandList& commandList)
{
	auto data = static_cast<VkCommandList::Data*>(commandList.GetCommandList());
	cmdBuffers_[nowSize_] = *data->commandBuffer_;
	nowSize_++;
}

void* Eugene::VkGpuEngine::GetQueue(void) const
{
	return const_cast<void*>(static_cast<const void*>(&queue_));
}
