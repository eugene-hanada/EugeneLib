#pragma once
#include <wrl.h>
#include <vector>
#include <d3d12.h>

namespace Eugene
{
	class CommandList;

	/// <summary>
	/// コマンドリストを実行するGPUエンジン(キュー)
	/// </summary>
	class GpuEngine
	{
	public:

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		GpuEngine() noexcept :
			fenceVal_{ 0 }
		{

		}

		/// <summary>
		/// デストラクタ
		/// </summary>
		~GpuEngine() = default;

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="gpuEngine"></param>
		GpuEngine(GpuEngine&& gpuEngine) noexcept:
			cmdQueue_{ std::move(gpuEngine.cmdQueue_ )}, fenceVal_{ gpuEngine.fenceVal_ }, fence_{ std::move(gpuEngine.fence_)}, commandLists_{std::move(gpuEngine.commandLists_)}
		{
			gpuEngine.fenceVal_ = 0;
			gpuEngine.commandLists_.clear();
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="gpuEngine"></param>
		/// <returns></returns>
		GpuEngine& operator=(GpuEngine&& gpuEngine) noexcept
		{
			cmdQueue_ = std::move(gpuEngine.cmdQueue_);
			
			fenceVal_ = gpuEngine.fenceVal_;
			gpuEngine.fenceVal_ = 0;

			fence_ = std::move(gpuEngine.fence_);

			commandLists_ = std::move(gpuEngine.commandLists_);

			return *this;
		}

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
		/// 
		/// </summary>
		/// <param name="initSize"></param>
		GpuEngine(std::size_t initSize) : fenceVal_{ 0 }
		{
			Init(initSize);
		}

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



