#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace Eugene
{
	class CommandList;

	/// <summary>
	/// コマンドリストを実行するGPUエンジン(キュー)
	/// </summary>
	class GpuEngine
	{
	public:
		GpuEngine() = default;

		~GpuEngine() = default;

		/// <summary>
		/// 保持しているコマンドリストを実行する
		/// </summary>
		void Execute(void);
		
		/// <summary>
		/// 実行完了まで待機する
		/// </summary>
		void Wait(void);
		
		/// <summary>
		/// 実行するコマンドリストをプッシュする
		/// </summary>
		/// <param name="commandList"> コマンドリスト </param>
		void Push(CommandList& commandList);

		/// <summary>
		/// API側のキューを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> ポインタ </returns>
		void* GetQueue(void) noexcept
		{
			return &queue_;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			cmdBuffers_.clear();
			queue_ = vk::Queue{};
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="gpuEngine"></param>
		GpuEngine(GpuEngine&& gpuEngine) noexcept:
			queue_{std::move(gpuEngine.queue_)}, cmdBuffers_{std::move(gpuEngine.cmdBuffers_)}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="gpuEngine"></param>
		/// <returns></returns>
		GpuEngine& operator=(GpuEngine&& gpuEngine) noexcept
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
