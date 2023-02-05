#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Math/Vector2.h"

struct ID3D12Device;
struct IDXGIFactory6;
struct IDXGISwapChain4;

namespace Eugene
{
	class Dx12Graphics :
		public Graphics
	{
	public:
		Dx12Graphics(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine, size_t bufferNum);
		~Dx12Graphics();
	
		
	private:
		GpuEngine* CreateGpuEngine(size_t maxSize) const final;

		CommandList* CreateCommandList(void) const final;
		GraphicsPipeline* CreateGraphicsPipeline(
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan samplerLayout = SamplerSpan{}) const final;

		GpuResource* CreateUploadableResource(size_t size) const final;
		GpuResource* CreateUploadableTextureResource(Image& texture) const final;

		GpuResource* CreateDefaultResource(size_t size) const final;
		GpuResource* CreateSwapChainResource(std::uint32_t idx) const final;
		GpuResource* CreateTextureResource(const TextureInfo& formatData) const final;
		GpuResource* CreateRenderTargetResource(const Vector2& size, Format format, const std::span<float, 4>& clearColor) const final;
		GpuResource* CreateDepthResource(const Vector2& size, Format format, float clearValue = 1.0f) const final;

		ShaderResourceViews* CreateShaderResourceViews(size_t size) const final;

		RenderTargetViews* CreateRenderTargetViews(size_t size, bool isShaderVisible) const final;

		DepthStencilViews* CreateDepthStencilViews(size_t size) const final;

		VertexView* CreateVertexView(size_t size, size_t vertexNum, GpuResource& resource) const final;

		void CreateDevice(void);

		void CreateSwapChain(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine, size_t bufferNum);

		void CreateBackBuffers(size_t bufferCount);

		GpuResource& GetBackBufferResource(size_t idx) final;

		RenderTargetViews& GetViews(void) final;

		size_t GetNowBackBufferIndex(void) final;

		void Present(void) final;

#ifdef USE_IMGUI
		void ImguiNewFrame(void) const final;
#endif

		// ファクトリ
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_{ nullptr };

		// DirectX12のデバイス
		Microsoft::WRL::ComPtr<ID3D12Device> device_{ nullptr };

		// スワップチェイン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_{ nullptr };
	
		std::vector<std::unique_ptr<GpuResource>> buffers_;
		std::unique_ptr<RenderTargetViews> renderTargetViews_;

#ifdef USE_IMGUI
		std::unique_ptr<ShaderResourceViews> srViews_;
		friend class Dx12CommandList;
#endif
	};
}
