#include "../../../Include/Graphics/DirectX12/Dx12GpuEngine.h"
#include <cassert>
#include "../../../Include/Graphics/CommandList.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include "../../../Include/Utils/EugeneLibException.h"

#include "../../../Include/Debug/Debug.h"



void Eugene::GpuEngine::Execute(void)
{
	cmdQueue_->ExecuteCommandLists(static_cast<UINT>(commandLists_.size()), reinterpret_cast<ID3D12CommandList* const*>(commandLists_.data()));
	commandLists_.clear();
}

void Eugene::GpuEngine::Wait(void)
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

void Eugene::GpuEngine::Push(CommandList& commandList)
{
	commandLists_.emplace_back(static_cast<ID3D12CommandList*>(commandList.GetCommandList()));
}


void Eugene::GpuEngine::Init(std::size_t initSize)
{
	commandLists_.reserve(initSize);

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
	if (FAILED(Graphics::GetInstance().device_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(cmdQueue_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("GpuEngineのコマンドキュー(ID3D12CommandQueue)作成失敗");
	}

	if (FAILED(Graphics::GetInstance().device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("GpuEngineのフェンス(ID3D12Fence)作成失敗");
	}
}