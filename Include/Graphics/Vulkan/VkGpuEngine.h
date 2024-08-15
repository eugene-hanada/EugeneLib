#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace Eugene
{
	class CommandList;
	class GpuEngine
	{
	public:
		GpuEngine() = default;

		~GpuEngine() = default;

		void Execute(void);
		
		void Wait(void);
		
		void Push(CommandList& commandList);

		void* GetQueue(void) const noexcept
		{
			return const_cast<void*>(static_cast<const void*>(&queue_));
		}

		void Final() noexcept
		{
			cmdBuffers_.clear();
			queue_ = vk::Queue{};
		}

		GpuEngine(GpuEngine&& gpuEngine) :
			queue_{std::move(gpuEngine.queue_)}, cmdBuffers_{std::move(gpuEngine.cmdBuffers_)}
		{
		}
		GpuEngine& operator=(GpuEngine&& gpuEngine)
		{
			queue_ = std::move(gpuEngine.queue_);
			cmdBuffers_ = std::move(gpuEngine.cmdBuffers_);
			return *this;
		}

		GpuEngine(const GpuEngine&) = delete;
		GpuEngine& operator=(const GpuEngine&) = delete;
	private:

		GpuEngine(std::size_t initSize);

		/// <summary>
		/// キュー
		/// </summary>
		vk::Queue queue_;
		
		/// <summary>
		/// 実行するコマンドバッファ
		/// </summary>
		std::vector<vk::CommandBuffer> cmdBuffers_;

		friend class Graphics;
	};
}
