#include "../../../Include/Graphics/Vulkan/VkGpuEngine.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"
#include "../../../Include/Graphics/Vulkan/VkCommandList.h"

void Eugene::GpuEngine::Execute(void)
{
	vk::SubmitInfo info{};
	info.setCommandBufferCount(cmdBuffers_.size());
	info.setPCommandBuffers(cmdBuffers_.data());
	queue_.submit(info);
	cmdBuffers_.clear();
}

void Eugene::GpuEngine::Wait(void)
{
	queue_.waitIdle();
}

void Eugene::GpuEngine::Push(CommandList& commandList)
{
	auto data = static_cast<vk::UniqueCommandBuffer*>(commandList.GetCommandList());
	cmdBuffers_.emplace_back(
		**data
	);

}

Eugene::GpuEngine::GpuEngine(std::size_t initSize)
{
	queue_ = Graphics::GetInstance().GetNextQueue();
	cmdBuffers_.reserve(initSize);
}