
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
#include "VkShaderResourceViews.h"
#include "VkSampler.h"
#include "VkSamplerViews.h"
#include "VkRenderTargetViews.h"

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
	
}
vk::Format Eugene::VkGraphics::CreateSwapChain(const Eugene::Vector2& size)
{
	vk::Format useFormat;
	auto capabilities = physicalDevice_.getSurfaceCapabilitiesKHR(*surfaceKhr_);
	auto format = physicalDevice_.getSurfaceFormatsKHR(*surfaceKhr_);
	auto modes = physicalDevice_.getSurfacePresentModesKHR(*surfaceKhr_);

	if (format.size() <= 0 || modes.size() <= 0)
	{
		// フォーマットもPresentも0以下ならスワップチェイン使えないのでやめる
		throw EugeneLibException{"対応フォーマットがありません"};
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

	info.setImageExtent({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y)});

	info.setImageArrayLayers(1);

	info.setImageColorSpace(vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear);

	info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst);

	info.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);

	info.setPresentMode(vk::PresentModeKHR::eFifo);

	info.setClipped(true);

	swapchain_ = device_->createSwapchainKHRUnique(info);

	return useFormat;

}
#endif

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
    return nullptr;
}

Eugene::ShaderResourceViews* Eugene::VkGraphics::CreateShaderResourceViews(std::uint64_t size) const
{
	throw EugeneLibException{"非対応です"};
    return nullptr;
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

std::uint64_t Eugene::VkGraphics::GetNowBackBufferIndex(void)
{
    return backBufferIdx_;
}

void Eugene::VkGraphics::Present(void)
{
	vk::SwapchainKHR sws[]{ *swapchain_ };
	vk::PresentInfoKHR info{};
	std::array<std::uint32_t, 2> idc{0u,1u};
	info.setImageIndices(backBufferIdx_);
	info.setSwapchains(*swapchain_);
	
	if (queue_.presentKHR(info) != vk::Result::eSuccess)
	{
		DebugLog("PresentError");
	}
	device_->resetFences(*fence_);
	
	
	if (device_->acquireNextImageKHR(*swapchain_, UINT64_MAX, {}, *fence_, &backBufferIdx_) != vk::Result::eSuccess)
	{
		DebugLog("acquireNextImageKHR Error");
	}

	if (device_->waitForFences(*fence_, true, UINT64_MAX) != vk::Result::eSuccess)
	{
		DebugLog("WaitFence Error");
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

Eugene::SamplerViews* Eugene::VkGraphics::CreateSamplerViews(std::uint64_t size) const
{
	throw EugeneLibException{ "非対応です" };
	return nullptr;
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
	//auto devices{  };


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

Eugene::GraphicsPipeline* Eugene::VkGraphics::CreateGraphicsPipeline(ShaderInputSpan layout, ShaderTypePaisrSpan shaders, RenderTargetSpan rendertarges, TopologyType topologyType, bool isCulling, bool useDepth, ShaderLayoutSpan shaderLayout, SamplerSpan samplerLayout) const
{
	return nullptr;
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

Eugene::IndexView* Eugene::VkGraphics::CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const
{
	throw EugeneLibException{"まだ実装していません"};
	return nullptr;
}
