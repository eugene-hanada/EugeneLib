#pragma once
#include <wrl.h>
#include <vector>
#include <d3d12.h>

namespace Eugene
{
	class CommandList;

	class GpuEngine
	{
	public:
		GpuEngine() noexcept :
			fenceVal_{ 0 }
		{

		}
		~GpuEngine() = default;
		GpuEngine(GpuEngine&& gpuEngine) noexcept:
			cmdQueue_{ std::move(gpuEngine.cmdQueue_ )}, fenceVal_{ gpuEngine.fenceVal_ }, fence_{ std::move(gpuEngine.fence_)}, commandLists_{std::move(gpuEngine.commandLists_)}
		{
			gpuEngine.fenceVal_ = 0;
			gpuEngine.commandLists_.clear();
		}

		GpuEngine& operator=(GpuEngine&& gpuEngine) noexcept
		{
			cmdQueue_ = std::move(gpuEngine.cmdQueue_);
			
			fenceVal_ = gpuEngine.fenceVal_;
			gpuEngine.fenceVal_ = 0;

			fence_ = std::move(gpuEngine.fence_);

			commandLists_ = std::move(gpuEngine.commandLists_);

			return *this;
		}

		GpuEngine(std::size_t initSize) : fenceVal_{ 0 }
		{
			Init(initSize);
		}

		void Execute(void);
		
		void Wait(void);

		void Push(CommandList& commandList);
		
		void* GetQueue(void) const noexcept
		{
			return cmdQueue_.Get();
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			cmdQueue_.Reset();
			fenceVal_ = 0;
			commandLists_.clear();
		}
	private:
		GpuEngine(const GpuEngine&) = delete;
		GpuEngine& operator=(const GpuEngine&) = delete;

		/// <summary>
		/// 初期化処理(遅延初期化用)
		/// </summary>
		/// <param name="initSize"></param>
		void Init(std::size_t initSize);

		// コマンドキュー
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue_{ nullptr };

		// フェンス値
		std::uint64_t fenceVal_;

		// フェンス
		Microsoft::WRL::ComPtr<ID3D12Fence> fence_{ nullptr };

		/// <summary>
		/// 実行するコマンドリスト
		/// </summary>
		std::vector<ID3D12CommandList*> commandLists_;

		friend class Graphics;
	};

};



