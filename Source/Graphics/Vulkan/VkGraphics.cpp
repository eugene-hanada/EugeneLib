
#ifdef USE_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "VkGraphics.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "VkGpuEngine.h"
#include  "../../../Include/Graphics/Shader.h"
#include "VkResourceBindLayout.h"
#include "VkCommandList.h"

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
	fence_ = std::make_shared<vk::UniqueFence>();
	*fence_ = device_->createFenceUnique(fenceInfo);

	vk::SemaphoreCreateInfo semphInfo{};
	semaphore_ = std::make_shared<vk::UniqueSemaphore>();
	*semaphore_ = device_->createSemaphoreUnique(semphInfo);

	queue_ = device_->getQueue(graphicFamilly_, nextQueueIdx_++);
	
	CreateSwapChain(size);

	gpuEngine = new VkGpuEngine{ queue_, fence_,semaphore_,maxNum };
}
void Eugene::VkGraphics::CreateSwapChain(const Eugene::Vector2& size)
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

	info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

	info.setImageSharingMode(vk::SharingMode::eExclusive);

	info.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);

	info.setPresentMode(vk::PresentModeKHR::eFifo);

	info.setClipped(true);

	swapchain_ = device_->createSwapchainKHRUnique(info);

}
#endif

Eugene::VkGraphics::~VkGraphics()
{
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
    return nullptr;
}

Eugene::BufferResource* Eugene::VkGraphics::CreateBufferResource(std::uint64_t size) const
{
    return nullptr;
}

Eugene::BufferResource* Eugene::VkGraphics::CreateBufferResource(Image& texture) const
{
    return nullptr;
}

Eugene::ImageResource* Eugene::VkGraphics::CreateImageResource(const TextureInfo& formatData) const
{
    return nullptr;
}

Eugene::ImageResource* Eugene::VkGraphics::CreateImageResource(const Vector2I& size, Format format, std::span<float, 4> color)
{
    return nullptr;
}

Eugene::ShaderResourceViews* Eugene::VkGraphics::CreateShaderResourceViews(std::uint64_t size) const
{
    return nullptr;
}

Eugene::DepthStencilViews* Eugene::VkGraphics::CreateDepthStencilViews(std::uint64_t size) const
{
    return nullptr;
}

Eugene::RenderTargetViews* Eugene::VkGraphics::CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const
{
    return nullptr;
}

Eugene::VertexView* Eugene::VkGraphics::CreateVertexView(std::uint64_t size, std::uint64_t vertexNum, BufferResource& resource) const
{
    return nullptr;
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
    return std::uint64_t();
}

void Eugene::VkGraphics::Present(void)
{
	vk::PresentInfoKHR info{};
	info.setImageIndices(backBufferIdx_);
	queue_.presentKHR(info);

	//device_->acquireNextImageKHR();
}

Eugene::Sampler* Eugene::VkGraphics::CreateSampler(const SamplerLayout& layout) const
{
    return nullptr;
}

Eugene::SamplerViews* Eugene::VkGraphics::CreateSamplerViews(std::uint64_t size) const
{
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
	return nullptr;
}

Eugene::ResourceBindLayout* Eugene::VkGraphics::CreateResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes) const
{
	return new VkResourceBindLayout{*device_,viewTypes};
}

Eugene::ImageResource* Eugene::VkGraphics::CreateDepthResource(const Vector2I& size, float clear) const
{
	return nullptr;
}

Eugene::IndexView* Eugene::VkGraphics::CreateIndexView(std::uint32_t size, std::uint32_t num, Format format, BufferResource& resource) const
{
	return nullptr;
}
