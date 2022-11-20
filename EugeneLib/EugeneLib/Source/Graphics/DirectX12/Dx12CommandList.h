#pragma once
#include <wrl.h>
#include "../../../Include/Graphics/CommandList.h"

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;

namespace EugeneLib
{
	class Dx12CommandList :
		public CommandList
	{
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		Dx12CommandList(Graphics& graphics);
		void* GetCommandList(void) const final;
	private:
		void Begin(void) final;
		void End(void) final;
		void SetRenderTarget(RenderTargetViews& views, size_t idx = 0) final;
		void SetRenderTarget(RenderTargetViews& views) final;
		void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, size_t rtViewsIdx = 0, size_t dsViewsIdx = 0)final;
		void SetRenderTarget(RenderTargetViews& views, DepthStencilViews& depthViews, size_t rtViewsIdx = 0) final;
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color, size_t idx = 0) final;
		void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) final;
		void TransitionRenderTargetBegin(GpuResource& resource) final;
		void TransitionRenderTargetEnd(GpuResource& resource) final;
		void Copy(GpuResource& destination, GpuResource& source) final;
		ComPtr< ID3D12CommandAllocator> cmdAllocator_;
		ComPtr< ID3D12GraphicsCommandList> cmdList_;

	};
}



