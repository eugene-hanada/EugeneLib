#pragma once
#include "../../../Include/Graphics/GpuEngine.h"
#include <wrl.h>
#include <vector>

struct ID3D12CommandList;
struct ID3D12CommandQueue;
struct ID3D12Fence;

namespace EugeneLib
{

	class Dx12GpuEngine :
		public GpuEngine
	{
	public:
		Dx12GpuEngine(size_t maxSize, Graphics& graphics);

	private:
		
		void Execute(void) final;
		void Wait(void) final;
		void Push(CommandList& commandList) final;
		void* GetQueue(void) const final;

		// �R�}���h�L���[
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue_{ nullptr };

		// �t�F���X�l
		std::uint64_t fenceVal_;

		// �t�F���X
		Microsoft::WRL::ComPtr<ID3D12Fence> fence_{ nullptr };

		std::vector<ID3D12CommandList*> commandLists_;

		std::uint64_t nowCommandNum_;
	};

};



