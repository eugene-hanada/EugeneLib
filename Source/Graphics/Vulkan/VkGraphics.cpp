
#ifdef USE_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "VkGraphics.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "VkGpuEngine.h"
#include  "../../../Include/Graphics/Shader.h"
#include "VkResourceBindLayout.h"
#include "VkCommandList.h"
#include "VkImageResource.h"
#include "VkBufferResource.h"
#include "VkDepthStencilViews.h"
#include "VkGraphicsPipeline.h"
#include "VkVertexView.h"
#include "VkIndexView.h"
#include "VkShaderResourceViews.h"
#include "VkSampler.h"
#include "VkSamplerViews.h"
#include "VkRenderTargetViews.h"

#ifdef USE_IMGUI
#include "../../../Include/ThirdParty/imgui/imgui.h"
#include "../../../Include/ThirdParty/imgui/backends/imgui_impl_vulkan.h"
#endif

#ifdef USE_EFFEKSEER
#include <Effekseer.h>
#include <EffekseerRendererVulkan.h>
#endif

#include "../../../Include/Common/Debug.h"

//#pragma comment(lib,"VkLayer_utils.lib")
//#pragma comment(lib,"vulkan-1.lib")

namespace
{
	std::uint32_t nextQueueIdx_ = 0;
}

#ifdef USE_WINDOWS
Eugene::VkGraphics::VkGraphics(HWND& hwnd, const Vector2& size, GpuEngine*& gpuEngine, std::uint32_t bufferNum, std::uint64_t maxNum) :
	backBufferIdx_{0}
{
	CreateInstance();

	CreateDevice();

	vk::Win32SurfaceCreateInfoKHR surfaceInfo{};
	surfaceInfo.setHinstance(GetModuleHandle(nullptr));
	surfaceInfo.setHwnd(hwnd);
	surfaceKhr_ = instance_->createWin32SurfaceKHRUnique(surfaceInfo);
	


	vk::FenceCreateInfo fenceInfo{};
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
	
	fence_ = device_->createFenceUnique(fenceInfo);

	queue_ = device_->getQueue(graphicFamilly_, nextQueueIdx_++);
	
	auto useVkformat = CreateSwapChain(size);

	gpuEngine = new VkGpuEngine{ queue_,maxNum };

	buffers_.resize(bufferNum);


	auto useFormat{ Format::NON };
	for (std::uint64_t i = 0ull; i < FormatMax; i++)
	{
		if (FormatToVkFormat[i] == useVkformat)
		{
			useFormat = static_cast<Format>(i);
			break;
		}
	}

	auto images = device_->getSwapchainImagesKHR(*swapchain_);
	//vk::UniqueImage i{images[0], vk::ObjectDestroy<vk::Device, vk::DispatchLoaderStatic>(*device_) };
	
	
	for (std::uint32_t i = 0u; i < bufferNum; i++)
	{
		buffers_[i] = std::make_unique<VkImageResource>(size, useFormat, images[i],*device_);
	}

	renderTargetViews_.reset(CreateRenderTargetViews(buffers_.size(),true));

	for (size_t i = 0ull; i < buffers_.size(); i++)
	{
		renderTargetViews_->Create(*buffers_[i], i);
	}
	
	
	device_->resetFences(*fence_);
	auto result = device_->acquireNextImageKHR(*swapchain_, UINT64_MAX,{}, *fence_, &backBufferIdx_);
	result = device_->waitForFences(*fence_,true,UINT64_MAX);
	device_->resetFences(*fence_);


#ifdef USE_IMGUI
	ImGui_ImplVulkan_InitInfo info{};
	info.
	ImGui_ImplVulkan_Init()
#endif


	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.setQueueFamilyIndex(graphicFamilly_);
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	effekseerPool_ = device_->createCommandPoolUnique(poolInfo);

	vk::AttachmentDescription colorAttachment{};
	colorAttachment.setFormat(useVkformat);
	colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
	colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
	colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
	colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
	colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
	vk::AttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
	
	vk::SubpassDescription subpass;
	subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
	subpass.setColorAttachmentCount(1);
	subpass.setPColorAttachments(&colorAttachmentRef);

	vk::RenderPassCreateInfo renderPassInfo{};
	renderPassInfo.setAttachmentCount(1);
	renderPassInfo.setPAttachments(&colorAttachment);
	renderPassInfo.setSubpassCount(1);
	renderPassInfo.setPSubpasses(&subpass);

	auto tmp = device_->createRenderPassUnique(renderPassInfo);
	frameBuffer_.resize(buffers_.size());
	auto& tmpRtViews = *static_cast<VkRenderTargetViews::ViewsType*>(renderTargetViews_->GetViews());
	for (auto i = 0ull; i < buffers_.size(); i++)
	{
		
		vk::ImageView attachments[] = {
			*tmpRtViews[i].imageView
		};
		vk::FramebufferCreateInfo info{};
		info.setRenderPass(*tmp);
		info.setAttachmentCount(1);
		info.setAttachments(attachments);
		info.setWidth(static_cast<std::uint32_t>(size.x));
		info.setHeight(static_cast<std::uint32_t>(size.y));
		info.setLayers(1);

		frameBuffer_[i] = device_->createFramebufferUnique(info);

	}

}
#endif

vk::Format Eugene::VkGraphics::CreateSwapChain(const Eugene::Vector2& size)
{
	vk::Format useFormat;
	auto capabilities = physicalDevice_.getSurfaceCapabilitiesKHR(*surfaceKhr_);
	auto format = physicalDevice_.getSurfaceFormatsKHR(*surfaceKhr_);
	auto modes = physicalDevice_.getSurfacePresentModesKHR(*surfaceKhr_);

	if (format.size() <= 0 || modes.size() <= 0)
	{
		// フォーマットもPresentも0以下ならスワップチェイン使えないのでやめる
		throw EugeneLibException{ "対応フォーマットがありません" };
	}

	auto itr = std::find_if(
		format.begin(), format.end(),
		[](vk::SurfaceFormatKHR& f) { return f.format == vk::Format::eB8G8R8A8Unorm || f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;  });
	if (itr != format.end())
	{
		useFormat = itr->format;
	}
	else
	{
		useFormat = format[0].format;
	}


	vk::SwapchainCreateInfoKHR info{};

	// サーフェスを指定
	info.setSurface(*surfaceKhr_);

	// バッファの数を指定
	info.setMinImageCount(2);

	// 一つのキューからの操作できる状態にする
	info.setImageSharingMode(vk::SharingMode::eExclusive);

	info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

	info.setImageFormat(useFormat);

	info.setImageExtent({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });

	info.setImageArrayLayers(1);

	info.setImageColorSpace(vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear);

	info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst);

	info.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);

	info.setPresentMode(vk::PresentModeKHR::eFifo);

	info.setClipped(true);

	swapchain_ = device_->createSwapchainKHRUnique(info);

	return useFormat;

}

Eugene::VkGraphics::~VkGraphics()
{
	queue_.waitIdle();
	device_->waitIdle();
}

vk::UniqueDeviceMemory Eugene::VkGraphics::CreateMemory(vk::UniqueImage& image) const
{
	auto memProps = physicalDevice_.getMemoryProperties();
	auto memRq = device_->getImageMemoryRequirements(*image);
	std::uint32_t heapIdx = 0u;
	std::uint32_t memIdx = 0u;

	// ヒープを探す
	for (std::uint32_t i = 0; i < memProps.memoryHeapCount; i++)
	{
		if (memProps.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal)
		{
			heapIdx = i;
			break;
		}
	}

	// メモリータイプを探す
	for (std::uint32_t i = 0; i < memProps.memoryTypeCount; i++)
	{
		if (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
		{
			if ((memRq.memoryTypeBits >> i) & 0x1)
			{
				if (memProps.memoryTypes[i].heapIndex == heapIdx)
				{
					memIdx = i;
					break;
				}
			}
		}
	}
	vk::MemoryAllocateInfo allocateInfo{};
	allocateInfo.setAllocationSize(memRq.size);
	allocateInfo.setMemoryTypeIndex(memIdx);
	return image.getOwner().allocateMemoryUnique(allocateInfo);
}

vk::UniqueDeviceMemory Eugene::VkGraphics::CreateMemory(vk::Buffer& buffer, bool isDeviceLoacal, bool isHostVisible) const
{
	auto memProps = physicalDevice_.getMemoryProperties();
	auto memRq = device_->getBufferMemoryRequirements(buffer);
	std::uint32_t heapIdx = 0u;
	std::uint32_t memIdx = 0u;

	// ヒープを探す
	for (std::uint32_t i = 0; i < memProps.memoryHeapCount; i++)
	{
		if (((memProps.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal) == vk::MemoryHeapFlagBits::eDeviceLocal) == isDeviceLoacal)
		{
			heapIdx = i;
			break;
		}
	}

	// メモリータイプを探す
	for (std::uint32_t i = 0; i < memProps.memoryTypeCount; i++)
	{
		auto propFlag = isHostVisible ? (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent) : vk::MemoryPropertyFlagBits::eDeviceLocal;
		if (memProps.memoryTypes[i].propertyFlags & propFlag)
		{
			if ((memRq.memoryTypeBits >> i) & 0x1)
			{
				if (memProps.memoryTypes[i].heapIndex == heapIdx)
				{
					memIdx = i;
					break;
				}
			}
		}
	}
	vk::MemoryAllocateInfo allocateInfo{};
	allocateInfo.setAllocationSize(memRq.size);
	allocateInfo.setMemoryTypeIndex(memIdx);
	return device_->allocateMemoryUnique(allocateInfo);
}

Eugene::GpuEngine* Eugene::VkGraphics::CreateGpuEngine(std::uint64_t maxSize) const
{
	return new VkGpuEngine{graphicFamilly_, nextQueueIdx_,*device_,maxSize};
}

Eugene::CommandList* Eugene::VkGraphics::CreateCommandList(void) const
{
	return new VkCommandList{*device_, graphicFamilly_ };
}

Eugene::BufferResource* Eugene::VkGraphics::CreateUploadableBufferResource(std::uint64_t size) const
{
	return new VkUploadableBufferResource{*device_, *this, size};
}

Eugene::BufferResource* Eugene::VkGraphics::CreateBufferResource(std::uint64_t size) const
{
	return new VkBufferResource{*device_, *this, size};
}

Eugene::BufferResource* Eugene::VkGraphics::CreateBufferResource(Image& texture) const
{
	return new VkUploadableBufferResource{*device_,*this, texture};
}

Eugene::ImageResource* Eugene::VkGraphics::CreateImageResource(const TextureInfo& formatData) const
{
	// 
	return new VkImageResource{*this, *device_,formatData};
}

Eugene::ImageResource* Eugene::VkGraphics::CreateImageResource(const Vector2I& size, Format format, std::span<float, 4> color)
{
	return new VkImageResource{ *this, *device_, size,format, };
}


Eugene::ShaderResourceViews* Eugene::VkGraphics::CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const
{
	return new VkShaderResourceViews{*device_, viewTypes};
}

Eugene::DepthStencilViews* Eugene::VkGraphics::CreateDepthStencilViews(std::uint64_t size) const
{
	return new VkDepthStencilView{*device_, size};
}

Eugene::RenderTargetViews* Eugene::VkGraphics::CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const
{
	return new VkRenderTargetViews{size};
}

Eugene::VertexView* Eugene::VkGraphics::CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const
{
	return new VkVertexView{size,vertexNum, *static_cast<VkBufferData*>(resource.GetResource())->buffer_};
}

Eugene::ImageResource& Eugene::VkGraphics::GetBackBufferResource(std::uint64_t idx)
{
	return *buffers_[idx];
}

Eugene::RenderTargetViews& Eugene::VkGraphics::GetViews(void)
{
	return *renderTargetViews_;
}

std::uint64_t Eugene::VkGraphics::GetNowBackBufferIndex(void) const
{
    return backBufferIdx_;
}

void Eugene::VkGraphics::Present(void)
{
	vk::SwapchainKHR sws[]{ *swapchain_ };
	vk::PresentInfoKHR info{};
	info.setImageIndices(backBufferIdx_);
	info.setSwapchains(*swapchain_);
	
	if (queue_.presentKHR(info) != vk::Result::eSuccess)
	{
		DebugLog("PresentError");
		return;
	}
	device_->resetFences(*fence_);
	
	
	if (device_->acquireNextImageKHR(*swapchain_, UINT64_MAX, {}, *fence_, &backBufferIdx_) != vk::Result::eSuccess)
	{
		DebugLog("acquireNextImageKHR Error");
		return;
	}

	if (device_->waitForFences(*fence_, true, UINT64_MAX) != vk::Result::eSuccess)
	{
		DebugLog("WaitFence Error");
		return;
	}

	device_->waitIdle();
	device_->resetFences(*fence_);
}

Eugene::Sampler* Eugene::VkGraphics::CreateSampler(const SamplerLayout& layout) const
{
	return new VkSampler{ *device_, layout };
}

Eugene::SamplerViews* Eugene::VkGraphics::CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const
{
	return new VkSamplerViews{*device_, viewTypes};
}


void Eugene::VkGraphics::CreateInstance(void)
{
	auto appInfo{ vk::ApplicationInfo{} };
	appInfo.setPApplicationName("vulkanTest");
	appInfo.setPEngineName("No Engine");
	appInfo.setEngineVersion(VK_MAKE_VERSION(1, 3, 0));
	appInfo.setApiVersion(VK_API_VERSION_1_3);

	const std::vector< const char* > layers{
		"VK_LAYER_KHRONOS_validation"
	};
	std::vector<const char*> extens{
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef USE_WINDOWS
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif

#ifdef _DEBUG
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
		VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
	};

	auto info{ vk::InstanceCreateInfo() };
	info.setPApplicationInfo(&appInfo);
	info.setEnabledLayerCount(layers.size());
	info.setPpEnabledLayerNames(layers.data());
	info.setPEnabledExtensionNames(extens);
	info.setEnabledExtensionCount(extens.size());
	instance_ = vk::createInstanceUnique(
		info
	);
}

void Eugene::VkGraphics::CreateDevice(void)
{
	for (const auto& d : instance_->enumeratePhysicalDevices())
	{
		const auto props = d.getProperties2();
		auto tmp = d.getFeatures();
		auto nameView{ std::string_view(props.properties.deviceName) };
		if (props.sType == vk::StructureType::ePhysicalDeviceProperties2)
		{
			physicalDevice_ = d;
			if (nameView.find("NVIDIA") || nameView.find("AMD"))
			{
				break;
			}
		}
	}

	if (!physicalDevice_)
	{
		throw CreateErrorException{"使用できる物理デバイスがありません"};
	}


	auto queueFamilyProp = physicalDevice_.getQueueFamilyProperties();
	size_t queueMax = 0ull;
	size_t idx = 0ull;
	for (size_t i = 0ull; i < queueFamilyProp.size(); i++)
	{
		if ((queueFamilyProp[i].queueFlags & (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)))
		{
			idx = i;
			queueMax = queueFamilyProp[i].queueCount;
			break;
		}
		throw EugeneLibException{ "グラフィックスを使用できるデバイスがありません" };
	}
	
	std::vector<float> priority(queueMax);
	std::fill(priority.begin(), priority.end(), 0.0f);

	// スワップチェインの機能を有効かするので
	std::vector<const char*> extension{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME
	};
	vk::DeviceQueueCreateInfo queueInfo{};
	queueInfo.setQueueCount(queueMax);
	queueInfo.setQueueFamilyIndex(idx);
	queueInfo.setQueuePriorities(priority);
	std::vector<vk::DeviceQueueCreateInfo> queues{
		queueInfo
	};

	auto info{ vk::DeviceCreateInfo{} };
	info.setQueueCreateInfoCount(queues.size());
	info.setPQueueCreateInfos(queues.data());
	info.setEnabledExtensionCount(extension.size());
	info.setPpEnabledExtensionNames(extension.data());
	vk::StructureChain createInfoChain{ info,
				vk::PhysicalDeviceDynamicRenderingFeatures{true}
	};
	device_ = physicalDevice_.createDeviceUnique(createInfoChain.get<vk::DeviceCreateInfo>());
	graphicFamilly_ = idx;
}

Eugene::GraphicsPipeline* Eugene::VkGraphics::CreateGraphicsPipeline(ResourceBindLayout& resourceBindLayout, const ArgsSpan<ShaderInputLayout>& layout, const ArgsSpan<ShaderPair>& shaders, const ArgsSpan<RendertargetLayout>& rendertarges, TopologyType topologyType, bool isCulling, bool useDepth) const
{
	return new VkGraphicsPipeline{*device_, resourceBindLayout, layout, shaders, rendertarges, topologyType, isCulling, useDepth};
}

Eugene::ResourceBindLayout* Eugene::VkGraphics::CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes) const
{
	return new VkResourceBindLayout{*device_,viewTypes};
}

Eugene::ImageResource* Eugene::VkGraphics::CreateDepthResource(const Vector2I& size, float clear) const
{
	return new VkImageResource{*this,*device_, size, clear};
}


#ifdef USE_EFFEKSEER

namespace Eugene
{
	class VkEffekseerWarpper :
		public EffekseerWarpper
	{
	public:
		VkEffekseerWarpper(
			const vk::PhysicalDevice& physicalDevice,
			const vk::Device& device,
			const vk::Queue& queue, 
			const vk::CommandPool& pool,
			std::uint32_t swapchainCount, vk::Format rtFormat, vk::Format depthFormtat, bool reverseDepth, std::uint64_t maxNum)
		{
			
			auto graphicsDevice = EffekseerRendererVulkan::CreateGraphicsDevice
			(
				VkPhysicalDevice{ physicalDevice },
				device,
				queue,
				pool,
				swapchainCount
			);

		
			EffekseerRendererVulkan::RenderPassInformation renderPassInfo;
			renderPassInfo.DoesPresentToScreen = false;
			renderPassInfo.RenderTextureCount = 1;
			renderPassInfo.RenderTextureFormats[0] = VK_FORMAT_B8G8R8A8_UNORM;
			//renderPassInfo.DepthFormat = VK_FORMAT_D32_SFLOAT;
			
			renderer_ = EffekseerRendererVulkan::Create(graphicsDevice, renderPassInfo, maxNum);
			manager_ = Effekseer::Manager::Create(maxNum);

			// レンダラーセット
			manager_->SetSpriteRenderer(renderer_->CreateSpriteRenderer());
			manager_->SetRibbonRenderer(renderer_->CreateRibbonRenderer());
			manager_->SetRingRenderer(renderer_->CreateRingRenderer());
			manager_->SetTrackRenderer(renderer_->CreateTrackRenderer());
			manager_->SetModelRenderer(renderer_->CreateModelRenderer());

			// ローダーセット
			manager_->SetTextureLoader(renderer_->CreateTextureLoader());
			manager_->SetModelLoader(renderer_->CreateModelLoader());
			manager_->SetMaterialLoader(renderer_->CreateMaterialLoader());

			// メモリープールとコマンドリストを生成
			memoryPool_ = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer_->GetGraphicsDevice());
			cmdList_ = EffekseerRenderer::CreateCommandList(renderer_->GetGraphicsDevice(), memoryPool_);
			renderer_->SetCommandList(cmdList_);

			auto viewerPosition = ::Effekseer::Vector3D(0.0f, 0.0f, -20.0f);
			renderer_->SetCameraMatrix(
				Effekseer::Matrix44().LookAtRH(
					viewerPosition, Effekseer::Vector3D(0.0f, 0.0f, 0.0f), Effekseer::Vector3D(0.0f, 1.0f, 0.0f)
				)
			);
		}

		// EffekseerWarpper を介して継承されました
		void Update(float delta) override
		{
			// 1フレームの経過時間を60フレーム基準での経過フレームに変換用
			constexpr auto frameParSec = 1.0f / 60.0f;

			// 開始処理
			memoryPool_->NewFrame();

			// 更新処理
			manager_->Update(delta / frameParSec);
		}

		void Draw(CommandList& cmdList) override
		{
			auto vkCmdList{ static_cast<vk::UniqueCommandBuffer*>(cmdList.GetCommandList()) };
			
			Effekseer::Manager::DrawParameter drawParameter;
			drawParameter.ZNear = 0.0f;
			drawParameter.ZFar = 1.0f;
			drawParameter.ViewProjectionMatrix = renderer_->GetCameraProjectionMatrix();
			
			EffekseerRendererVulkan::BeginCommandList(cmdList_, **vkCmdList);
			renderer_->SetCommandList(cmdList_);
			renderer_->BeginRendering();
			manager_->Draw(drawParameter);
			renderer_->EndRendering();
			renderer_->SetCommandList(nullptr);
			EffekseerRendererVulkan::EndCommandList(cmdList_);
		}
		Effekseer::RefPtr<Effekseer::Manager>& GetManager() & final
		{
			return manager_;

		}
		void SetCameraPos(const Vector3& eye, const Vector3& at, const Vector3& up) final
		{
			renderer_->SetCameraMatrix(
				Effekseer::Matrix44().LookAtLH(
					Effekseer::Vector3D{ eye.x,eye.y, eye.z }, Effekseer::Vector3D{ at.x, at.y, at.z }, Effekseer::Vector3D{ up.x, up.y, up.z }
				)
			);
		}
		void SetCameraProjection(float fov, float aspect, const Eugene::Vector2& nearfar) final
		{
			renderer_->SetProjectionMatrix(
				Effekseer::Matrix44().PerspectiveFovLH(fov, aspect, nearfar.x, nearfar.y));
		}

	private:

		/// <summary>
		/// レンダラー
		/// </summary>
		EffekseerRenderer::RendererRef renderer_;

		/// <summary>
		/// マネージャー
		/// </summary>
		Effekseer::RefPtr<Effekseer::Manager> manager_;

		/// <summary>
		/// メモリプール
		/// </summary>
		Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> memoryPool_;

		/// <summary>
		/// コマンドリスト
		/// </summary>
		Effekseer::RefPtr<EffekseerRenderer::CommandList> cmdList_;
	};
}

Eugene::EffekseerWarpper* Eugene::VkGraphics::CreateEffekseerWarpper(GpuEngine& gpuEngine, Format rtFormat, std::uint32_t rtNum, Format depthFormat, bool reverseDepth, std::uint64_t maxNumm) const
{

	return new VkEffekseerWarpper{
		physicalDevice_,
		*device_,
		queue_,
		*effekseerPool_,
		rtNum,
		FormatToVkFormat[static_cast<size_t>(rtFormat)],
		FormatToVkFormat[static_cast<size_t>(depthFormat)],
		reverseDepth,
		maxNumm
	};
}
#endif

Eugene::IndexView* Eugene::VkGraphics::CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const
{
	return new VkIndexView{ size, num, *static_cast<VkBufferData*>(resource.GetResource())->buffer_ };
}
