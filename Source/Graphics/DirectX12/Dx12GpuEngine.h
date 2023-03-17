﻿#pragma once
#include "../../../Include/Graphics/GpuEngine.h"
#include <wrl.h>
#include <vector>

struct ID3D12Device;
struct ID3D12CommandList;
struct ID3D12CommandQueue;
struct ID3D12Fence;

namespace Eugene
{

	class Dx12GpuEngine :
		public GpuEngine
	{
	public:
		Dx12GpuEngine(ID3D12Device* device, size_t maxSize);
		~Dx12GpuEngine();
	private:
		
		void Execute(void) final;
		void Wait(void) final;
		void Push(CommandList& commandList) final;
		void* GetQueue(void) const final;

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

		/// <summary>
		/// 現在の実行すべきコマンドリストの数
		/// </summary>
		std::uint64_t nowCommandNum_;
	};

};



