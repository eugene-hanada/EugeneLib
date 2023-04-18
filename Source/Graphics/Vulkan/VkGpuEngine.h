#pragma once
#include "../../../Include/Graphics/GpuEngine.h"
#include <vulkan/vulkan.hpp>
#include <vector>

namespace Eugene
{


	class VkGpuEngine :
		public GpuEngine
	{
	public:
		VkGpuEngine(vk::Queue& queue, std::shared_ptr<vk::UniqueFence>& fence, std::shared_ptr<vk::UniqueSemaphore>& semaphore, std::uint64_t size);
		VkGpuEngine(std::uint32_t familyIndex,std::uint32_t& queueIndex, const vk::Device& device, std::uint64_t size);
		~VkGpuEngine();
		void Execute(void) final;
		void Wait(void) final;
		void Push(CommandList& commandList) final;
		void* GetQueue(void) const final;
	private:
		vk::Queue queue_;
		std::shared_ptr<vk::UniqueFence> fence_;
		std::shared_ptr<vk::UniqueSemaphore> semaphore_;
		std::vector<vk::CommandBuffer> cmdBuffers_;
		std::uint64_t nowSize_;
	};
}
