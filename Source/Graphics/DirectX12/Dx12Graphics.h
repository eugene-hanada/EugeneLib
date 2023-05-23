#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include <array>
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
		static const std::array<int, FormatMax> FormatToDxgiFormat_;
		Dx12Graphics(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);
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
			bool useDepth = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan samplerLayout = SamplerSpan{}
			) const final;

		BufferResource* CreateUploadableBufferResource(std::uint64_t size) const final;

		BufferResource* CreateBufferResource(std::uint64_t size) const final;

		BufferResource* CreateBufferResource(Image& texture) const final;

		ImageResource* CreateImageResource(const TextureInfo& formatData) const final;

		ImageResource* CreateImageResource(const Vector2I& size, Format format, std::span<float, 4> clearColor) final;

		ImageResource* CreateDepthResource(const Vector2I& size, float clear) const final;

		ShaderResourceViews* CreateShaderResourceViews(std::uint64_t size) const final;

		RenderTargetViews* CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const final;

		DepthStencilViews* CreateDepthStencilViews(std::uint64_t size) const final;

		VertexView* CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const final;

		IndexView* CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const final;

		void CreateDevice(void);

		void CreateSwapChain(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum);

		void CreateBackBuffers(std::uint64_t bufferCount);

		ImageResource& GetBackBufferResource(std::uint64_t idx) final;

		RenderTargetViews& GetViews(void) final;

		std::uint64_t GetNowBackBufferIndex(void) final;

		void Present(void) final;

		Sampler* CreateSampler(const SamplerLayout& layout) const final;

		SamplerViews* CreateSamplerViews(std::uint64_t size) const final;

		void ResizeBackBuffer(const Vector2& size) final;

		void SetFullScreenFlag(bool isFullScreen) final;
#ifdef USE_IMGUI
		void ImguiNewFrame(void) const final;
		void* GetImguiImageID(std::uint64_t index) const final;
		ShaderResourceViews& GetImguiShaderResourceView(void) & final;
#endif

		// dxgi
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory_{ nullptr };

		// DirectX12デバイス
		Microsoft::WRL::ComPtr<ID3D12Device> device_{ nullptr };

		// スワップチェイン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_{ nullptr };
	
		std::vector<std::unique_ptr<ImageResource>> buffers_;
		std::unique_ptr<RenderTargetViews> renderTargetViews_;

#ifdef USE_IMGUI
		std::unique_ptr<ShaderResourceViews> srViews_;
#endif

#ifdef USE_Effekseer
		EffekseerWarpper* CreateEffekseerWarpper(
			GpuEngine& gpuEngine,
			Format rtFormat,
			std::uint32_t rtNum,
			bool reverseDepth = false,
			std::uint64_t maxNumm = 8000
		) const final;
#endif
		friend class Dx12CommandList;
	};
}
