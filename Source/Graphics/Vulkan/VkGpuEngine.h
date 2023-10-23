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
		VkGpuEngine(vk::Queue& queue, std::uint64_t size);
		VkGpuEngine(std::uint32_t familyIndex,std::uint32_t& queueIndex, const vk::Device& device, std::uint64_t size);
		~VkGpuEngine();
		void Execute(void) final;
		void Wait(void) final;
		void Push(CommandList& commandList) final;
		void* GetQueue(void) const final;
	private:

		/// <summary>
		/// �L���[
		/// </summary>
		vk::Queue queue_;
		
		/// <summary>
		/// ���s����R�}���h�o�b�t�@
		/// </summary>
		std::vector<vk::CommandBuffer> cmdBuffers_;

		/// <summary>
		/// ���݂̎��s����R�}���h�o�b�t�@�̐�
		/// </summary>
		std::uint64_t nowNum_;
	};
}
