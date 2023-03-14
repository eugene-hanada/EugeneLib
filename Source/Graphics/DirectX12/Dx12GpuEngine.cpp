#include "Dx12GpuEngine.h"
#include <cassert>
#include <d3d12.h>
#include "../../../Include/Graphics/CommandList.h"
#include "Dx12Graphics.h"
#include "../../../Include/Common/EugeneLibException.h"

#include "../../../Include/Common/Debug.h"


Eugene::Dx12GpuEngine::Dx12GpuEngine(ID3D12Device* device,size_t maxSize) :
	fenceVal_{0}
{
	commandLists_.resize(maxSize);
	nowCommandNum_ = 0ull;
	// 設定をDESC構造体で行う
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

	// タイムアウトの設定(無しにする)
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// アダプターの設定(一つしか使わないときは0でいい)
	cmdQueueDesc.NodeMask = 0;

	// プライオリティ特に設定なし
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	// ここはコマンドリストと合わせる
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	
	// キューを生成
	if (FAILED(device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(cmdQueue_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException("GpuEngineのコマンドキュー作成失敗");
	}

	if (FAILED(device->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException("GpuEngineのフェンス作成失敗");
	}
}

Eugene::Dx12GpuEngine::~Dx12GpuEngine()
{
}

//	void GpuEngine::ReSize(size_t size)
//	{
//		cmdlists_.resize(size);
//		std::fill(cmdlists_.begin(), cmdlists_.end(), nullptr);
//	}
//}
void Eugene::Dx12GpuEngine::Execute(void)
{
	auto p = commandLists_.data();
	cmdQueue_->ExecuteCommandLists(static_cast<UINT>(nowCommandNum_), reinterpret_cast<ID3D12CommandList* const*>(p));
	nowCommandNum_ = 0ull;
	std::fill(commandLists_.begin(), commandLists_.end(), nullptr);
}

void Eugene::Dx12GpuEngine::Wait(void)
{
	cmdQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() < fenceVal_)
	{
		// イベントを取得
		auto ev = CreateEvent(nullptr, false, false, nullptr);

		if (ev != 0)
		{
			// イベントをセット
			fence_->SetEventOnCompletion(fenceVal_, ev);

			// イベントが発生するまで待つ
			WaitForSingleObject(ev, INFINITE);

			// イベントを閉じる
			CloseHandle(ev);
		}
	}
}

void Eugene::Dx12GpuEngine::Push(CommandList& commandList)
{
	if (nowCommandNum_ >= commandLists_.size())
	{
		return;
	}
	commandLists_[nowCommandNum_] = static_cast<ID3D12CommandList*>(commandList.GetCommandList());
	nowCommandNum_++;
}

void* Eugene::Dx12GpuEngine::GetQueue(void) const
{
	return cmdQueue_.Get();
}

