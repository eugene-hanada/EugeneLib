#include "Dx12GpuEngine.h"
#include <cassert>
#include <d3d12.h>
#include "../../../Include/Graphics/CommandList.h"
#include "Dx12Graphics.h"
#include "../../../Include/Common/EugeneLibException.h"

#include "../../../Include/Common/Debug.h"

//namespace EugeneLib
//{
//
//
//	GpuEngine::GpuEngine(const CmdType cmdType, size_t maxCmd) :
//		fenceVal_{ 0 }, nowSize_{ 0U }, cmdType_{ cmdType }, maxCmd_{ maxCmd }
//	{
//
//		cmdlists_.resize(10);
//		// �ݒ��DESC�\���̂ōs��
//		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
//
//		// �^�C���A�E�g�̐ݒ�(�����ɂ���)
//		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//
//		// �A�_�v�^�[�̐ݒ�(������g��Ȃ��Ƃ���0�ł���)
//		cmdQueueDesc.NodeMask = 0;
//
//		// �v���C�I���e�B���ɐݒ�Ȃ�
//		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
//
//		// �����̓R�}���h���X�g�ƍ��킹��
//		cmdQueueDesc.Type = static_cast<D3D12_COMMAND_LIST_TYPE>(cmdType_);
//
//		// �L���[�𐶐�
//		if (FAILED(EugeneLib::System::Ptr()->GetDevice()->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(cmdQueue_.ReleaseAndGetAddressOf()))))
//		{
//			throw EugeneLibException("GpuEngine�̃R�}���h�L���[�쐬���s");
//		}
//
//		if (FAILED(EugeneLib::System::Ptr()->GetDevice()->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf()))))
//		{
//			throw EugeneLibException("GpuEngine�̃t�F���X�쐬���s");
//		}
//	}
//
//	GpuEngine::~GpuEngine()
//	{
//	}
//
//	GpuEngine::GpuEngine()
//	{
//	}
//
//	void GpuEngine::Push(CmdList& cmdList)
//	{
//		if (cmdList.GetCmdType() == cmdType_ || maxCmd_ <= nowSize_)
//		{
//			return;
//		}
//		cmdlists_[nowSize_] = cmdList.GetCmdLilst().Get();
//		nowSize_++;
//	}
//
//	void EugeneLib::GpuEngine::Execute(void)
//	{
//		if (nowSize_ == 0U)
//		{
//			return;
//		}
//		auto p = cmdlists_.data();
//		cmdQueue_->ExecuteCommandLists(static_cast<UINT>(nowSize_), reinterpret_cast<ID3D12CommandList* const*>(p));
//		std::fill(cmdlists_.begin(), cmdlists_.end(), nullptr);
//		nowSize_ = 0U;
//	}
//
//	void EugeneLib::GpuEngine::Wait(void)
//	{
//		auto a = cmdQueue_->Signal(fence_.Get(), ++fenceVal_);
//		if (fence_->GetCompletedValue() < fenceVal_)
//		{
//			// �C�x���g���擾
//			auto ev = CreateEvent(nullptr, false, false, nullptr);
//
//			// �C�x���g���Z�b�g
//			fence_->SetEventOnCompletion(fenceVal_, ev);
//
//			// �C�x���g����������܂ő҂�
//			WaitForSingleObject(ev, INFINITE);
//
//			// �C�x���g�����
//			CloseHandle(ev);
//		}
//	}

EugeneLib::Dx12GpuEngine::Dx12GpuEngine(ID3D12Device* device,size_t maxSize) :
	fenceVal_{0}
{
	commandLists_.resize(maxSize);
	nowCommandNum_ = 0ull;
	// �ݒ��DESC�\���̂ōs��
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

	// �^�C���A�E�g�̐ݒ�(�����ɂ���)
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// �A�_�v�^�[�̐ݒ�(������g��Ȃ��Ƃ���0�ł���)
	cmdQueueDesc.NodeMask = 0;

	// �v���C�I���e�B���ɐݒ�Ȃ�
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	// �����̓R�}���h���X�g�ƍ��킹��
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	
	// �L���[�𐶐�
	if (FAILED(device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(cmdQueue_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException("GpuEngine�̃R�}���h�L���[�쐬���s");
	}

	if (FAILED(device->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException("GpuEngine�̃t�F���X�쐬���s");
	}
}

EugeneLib::Dx12GpuEngine::~Dx12GpuEngine()
{
	DebugLog(u8"gpuEngine");
}

//	void GpuEngine::ReSize(size_t size)
//	{
//		cmdlists_.resize(size);
//		std::fill(cmdlists_.begin(), cmdlists_.end(), nullptr);
//	}
//}
void EugeneLib::Dx12GpuEngine::Execute(void)
{
	auto p = commandLists_.data();
	cmdQueue_->ExecuteCommandLists(static_cast<UINT>(nowCommandNum_), reinterpret_cast<ID3D12CommandList* const*>(p));
	nowCommandNum_ = 0ull;
	std::fill(commandLists_.begin(), commandLists_.end(), nullptr);
}

void EugeneLib::Dx12GpuEngine::Wait(void)
{
	cmdQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() < fenceVal_)
	{
		// �C�x���g���擾
		auto ev = CreateEvent(nullptr, false, false, nullptr);

		if (ev != 0)
		{
			// �C�x���g���Z�b�g
			fence_->SetEventOnCompletion(fenceVal_, ev);

			// �C�x���g����������܂ő҂�
			WaitForSingleObject(ev, INFINITE);

			// �C�x���g�����
			CloseHandle(ev);
		}
	}
}

void EugeneLib::Dx12GpuEngine::Push(CommandList& commandList)
{
	if (nowCommandNum_ >= commandLists_.size())
	{
		return;
	}
	commandLists_[nowCommandNum_] = static_cast<ID3D12CommandList*>(commandList.GetCommandList());
	nowCommandNum_++;
}

void* EugeneLib::Dx12GpuEngine::GetQueue(void) const
{
	return cmdQueue_.Get();
}

