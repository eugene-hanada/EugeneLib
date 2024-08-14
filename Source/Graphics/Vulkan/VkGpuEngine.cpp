#include "../../../Include/Graphics/Vulkan/VkGpuEngine.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"
#include "../../../Include/Graphics/Vulkan/VkCommandList.h"

//Eugene::GpuEngine::GpuEngine(vk::Queue& queue, std::uint64_t size) :
//	queue_{queue}
//{
//	nowNum_ = 0ull;
//	cmdBuffers_.resize(size);
//}

//Eugene::GpuEngine::GpuEngine(std::uint32_t familyIndex, std::uint32_t& queueIndex, const vk::Device& device, std::uint64_t size)
//{
//	queue_ = device.getQueue(familyIndex, queueIndex++);
//
//	nowNum_ = 0ull;
//	cmdBuffers_.resize(size);
//}

//Eugene::GpuEngine::~GpuEngine()
//{ 
//}

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