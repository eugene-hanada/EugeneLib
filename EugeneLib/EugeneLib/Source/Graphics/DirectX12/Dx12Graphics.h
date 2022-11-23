#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include "../../../Include/Graphics/Graphics.h"


struct ID3D12Device;
struct IDXGIFactory6;
struct IDXGISwapChain4;

namespace EugeneLib
{
	class RenderTargetViews;
	class GpuResource;

	class Dx12Graphics :
		public Graphics
	{
	public:
		Dx12Graphics(System& system, GpuEngine*& gpuEngine);
		~Dx12Graphics();
	private:
		void* GetDevice(void) const final;

		void* GetSwapChain(void) const final;

		void CreateDevice(void);

		void CreateSwapChain(System& system, GpuEngine*& gpuEngine);

		void CreateBackBuffers(size_t bufferCount);

		GpuResource& GetBackBufferResource(void) final;

		RenderTargetViews& GetViews(void) final;

		size_t GetNowBackBufferIndex(void) final;

		void Present(void) final;

		// DirectX12のデバイス
		Microsoft::WRL::ComPtr<ID3D12Device> device_{ nullptr };
		// ファクトリ
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_{ nullptr };

		// スワップチェイン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_{ nullptr };
	
		std::vector<std::unique_ptr<GpuResource>> buffers_;
		std::unique_ptr<RenderTargetViews> renderTargetViews_;
	};
}
