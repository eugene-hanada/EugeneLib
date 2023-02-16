#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Math/Vector2.h"
#include "../../../Include/Graphics/GraphicsCommon.h"

struct ID3D12Device;
struct IDXGIFactory6;
struct IDXGISwapChain4;

namespace Eugene
{
	class Dx12Graphics :
		public Graphics
	{
	public:
		Dx12Graphics(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine, std::uint64_t bufferNum);
		~Dx12Graphics();
	
		
	private:
		GpuEngine* CreateGpuEngine(std::uint64_t maxSize) const final;

		CommandList* CreateCommandList(void) const final;
		GraphicsPipeline* CreateGraphicsPipeline(
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan samplerLayout = SamplerSpan{}) const final;

		BufferResource* CreateUploadableBufferResource(std::uint64_t size) const final;

		BufferResource* CreateBufferResource(std::uint64_t size) const final;

		BufferResource* CreateBufferResource(Image& texture) const final;

		ImageResource* CreateImageResource(const TextureInfo& formatData) const final;

		ShaderResourceViews* CreateShaderResourceViews(std::uint64_t size) const final;

		RenderTargetViews* CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const final;

		DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const final;

		VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const final;
		IndexView* CreateIndexView(std::uint64_t size, Format format, BufferResource& resource) const final;

		void CreateDevice(void);

		void CreateSwapChain(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine, std::uint64_t bufferNum);

		void CreateBackBuffers(std::uint64_t bufferCount);

		ImageResource& GetBackBufferResource(std::uint64_t idx) final;

		RenderTargetViews& GetViews(void) final;

		std::uint64_t GetNowBackBufferIndex(void) final;

		void Present(void) final;

		Sampler* CreateSampler(const SamplerLayout& layout) const final;

		SamplerViews* CreateSamplerViews(std::uint64_t size) const final;
#ifdef USE_IMGUI
		void ImguiNewFrame(void) const final;
#endif

		// ファクトリ
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_{ nullptr };

		// DirectX12のデバイス
		Microsoft::WRL::ComPtr<ID3D12Device> device_{ nullptr };

		// スワップチェイン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_{ nullptr };
	
		std::vector<std::unique_ptr<ImageResource>> buffers_;
		std::unique_ptr<RenderTargetViews> renderTargetViews_;

#ifdef USE_IMGUI
		std::unique_ptr<ShaderResourceViews> srViews_;
		friend class Dx12CommandList;
#endif
	};
}
