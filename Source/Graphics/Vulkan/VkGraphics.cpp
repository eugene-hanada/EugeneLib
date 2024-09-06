
#ifdef EUGENE_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#elif EUGENE_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#include <game-activity/native_app_glue/android_native_app_glue.h>
#endif

#define VMA_IMPLEMENTATION
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"
#include "../../../Include/Utils/EugeneLibException.h"

#include "../../../Include/System/System.h"

#ifdef USE_IMGUI
#include "../../../Include/ThirdParty/imgui/imgui.h"
#include "../../../Include/ThirdParty/imgui/backends/imgui_impl_vulkan.h"

#ifdef EUGENE_WINDOWS
#include "../../../Include/ThirdParty/imgui/backends/imgui_impl_win32.h"
#endif
#endif


#include "../../../Include/Debug/Debug.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE


namespace
{
#ifdef USE_IMGUI
	void CheckVkResult(VkResult err)
	{
		if (err != VkResult::VK_SUCCESS)
		{
			throw Eugene::EugeneLibException{ "Vulkan Result is not Success" };
		}
	
	}

	ImGui_ImplVulkanH_Window imguiWindowH{};

#ifdef EUGENE_WINDOWS
	VkAllocationCallbacks* callbacks = nullptr;
	int ImGui_ImplVulkan_CreateVkSurface(ImGuiViewport* viewport, ImU64 vk_instance, const void* vk_allocator, ImU64* out_vk_surface)
	{
		VkWin32SurfaceCreateInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		info.hinstance = GetModuleHandle(nullptr);
		info.hwnd = static_cast<HWND>(viewport->PlatformHandle);
		return static_cast<int>((VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateWin32SurfaceKHR(reinterpret_cast<VkInstance>(vk_instance), &info, reinterpret_cast<const VkAllocationCallbacks*>(vk_allocator), reinterpret_cast<VkSurfaceKHR*>(out_vk_surface))));
	}
#endif
#endif

#ifdef EUGENE_WINDOWS
	HWND hWindow;
#else EUGENE_ANDROID
    android_app* pApp{nullptr};
#endif
}

#ifdef EUGENE_WINDOWS
Eugene::Graphics::Graphics(GpuEngine& gpuEngine, std::uint32_t bufferNum, std::size_t maxNum):
	backBufferIdx_{0}, isMinimized{false}
{
	EUGENE_ASSERT_MSG(System::IsCreate(), "Systemが生成されていません。");

	DynamicSingleton::instance_.reset(this);
	hWindow = static_cast<HWND>(System::GetInstance().GetWindow());;
	auto size = System::GetInstance().GetWindowSize();
	CreateInstance();

	CreateDevice();

    SetUpVma();

	vk::Win32SurfaceCreateInfoKHR surfaceInfo{};
	surfaceInfo.setHinstance(GetModuleHandle(nullptr));
	surfaceInfo.setHwnd(hWindow);
	surfaceKhr_ = instance_->createWin32SurfaceKHRUnique(surfaceInfo);
	


	vk::FenceCreateInfo fenceInfo{};
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
	
	fence_ = device_->createFenceUnique(fenceInfo);

	queue_ = device_->getQueue(graphicFamilly_, nextQueueIdx_++);
	
	auto useVkformat = CreateSwapChain(size);

	gpuEngine = GpuEngine{ maxNum };

	buffers_.resize(bufferNum);


	CreateBackBuffer(useVkformat, size);
	
	
	device_->resetFences(*fence_);
	if (device_->acquireNextImageKHR(*swapchain_, std::numeric_limits<std::uint64_t>::max(), {}, *fence_, &backBufferIdx_) != vk::Result::eSuccess)
	{
		throw EugeneLibException{"AcquireNextImageKHR Error"};
	}

	if (device_->waitForFences(*fence_, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
	{
		throw EugeneLibException{ "WaitForFences Error" };
	}
	device_->resetFences(*fence_);

	const auto& limits{ physicalDevice_.getProperties().limits };
	auto flag = limits.framebufferColorSampleCounts & limits.framebufferDepthSampleCounts;
	
	if (flag & vk::SampleCountFlagBits::e64)
	{
		multiSampleCount_ = 64;
	}
	else if(flag & vk::SampleCountFlagBits::e32)
	{
		multiSampleCount_ = 32;
	}
	else if (flag & vk::SampleCountFlagBits::e16)
	{
		multiSampleCount_ = 16;
	}
	else if (flag & vk::SampleCountFlagBits::e8)
	{
		multiSampleCount_ = 8;
	}
	else if (flag & vk::SampleCountFlagBits::e4)
	{
		multiSampleCount_ = 4;
	}
	else if (flag & vk::SampleCountFlagBits::e2)
	{
		multiSampleCount_ = 2;
	}
	else
	{
		multiSampleCount_ = 1;
	}

	

#ifdef USE_IMGUI
	InitImgui(useVkformat, bufferNum, size);
#endif
}
#else  EUGENE_ANDROID
Eugene::Graphics::Graphics(GpuEngine& gpuEngine, std::uint32_t bufferNum, std::size_t maxNum)
{
	DynamicSingleton::instance_.reset(this);
    pApp = static_cast<android_app*>(System::GetInstance().GetWindow());

    // インスタンス生成
    CreateInstance();

    // デバイス生成
    CreateDevice();

    // Vmaをセットアップ
    SetUpVma();

    vk::AndroidSurfaceCreateInfoKHR surfaceInfo{};
    surfaceInfo.setWindow(pApp->window);
    surfaceKhr_ = instance_->createAndroidSurfaceKHRUnique(surfaceInfo);

    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    fence_ = device_->createFenceUnique(fenceInfo);
    queue_ = device_->getQueue(graphicFamilly_, nextQueueIdx_++);

    auto size = System::GetInstance().GetWindowSize();

    auto useVkformat = CreateSwapChain(size);

    gpuEngine = GpuEngine{maxNum };

    buffers_.resize(bufferNum);


    CreateBackBuffer(useVkformat, size);


    device_->resetFences(*fence_);
    auto result = device_->acquireNextImageKHR(*swapchain_, UINT64_MAX,{}, *fence_, &backBufferIdx_);
    result = device_->waitForFences(*fence_,true,UINT64_MAX);
    device_->resetFences(*fence_);
}

#endif



void Eugene::Graphics::CreateBackBuffer(vk::Format useVkformat, const glm::vec2& size)
{
	if (isMinimized)
	{
		return;
	}

	for (std::uint64_t i = 0ull; i < FormatMax; i++)
	{
		if (FormatToVkFormat[i] == useVkformat)
		{
			backBufferFormat_ = static_cast<Format>(i);
			break;
		}
	}

	auto images = device_->getSwapchainImagesKHR(*swapchain_);
	renderTargetViews_ = {static_cast<std::uint32_t>(buffers_.size())};
	for (std::uint64_t i = 0u; i < buffers_.size(); i++)
	{
		buffers_[i] = ImageResource{ size, backBufferFormat_, images[i] };
		renderTargetViews_.Create(buffers_[i], i);
	}
}

vk::Format Eugene::Graphics::CreateSwapChain(const glm::vec2& size)
{


	vk::Format useFormat;
	auto capabilities = physicalDevice_.getSurfaceCapabilitiesKHR(*surfaceKhr_);
	auto clampSize = glm::clamp(size, glm::vec2{ capabilities.minImageExtent.width, capabilities.minImageExtent.height }, glm::vec2{ capabilities.maxImageExtent.width, capabilities.maxImageExtent.height });
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

	if (isMinimized)
	{
		return useFormat;
	}

#ifdef USE_WINDOWS
	vk::SurfaceFullScreenExclusiveInfoEXT fullscrInfo{};
	fullscrInfo.setFullScreenExclusive(vk::FullScreenExclusiveEXT::eDefault);
	vk::SurfaceFullScreenExclusiveWin32InfoEXT fullscrWin32Info{};
	fullscrWin32Info.setHmonitor(MonitorFromWindow(hWindow, MONITOR_DEFAULTTONEAREST));
	fullscrInfo.setPNext(&fullscrWin32Info);
#endif

	vk::SwapchainCreateInfoKHR info{};

#ifdef USE_WINDOWS
	info.setPNext(&fullscrInfo);
#endif

	// サーフェスを指定
	info.setSurface(*surfaceKhr_);

	// バッファの数を指定
	info.setMinImageCount(2);

	// 一つのキューからの操作できる状態にする
	info.setImageSharingMode(vk::SharingMode::eExclusive);

    constexpr  vk::CompositeAlphaFlagBitsKHR checkCompositeAlphaArray[] = {
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
            vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
            vk::CompositeAlphaFlagBitsKHR::eInherit,
    };

    auto compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eInherit;

    for (auto flag : checkCompositeAlphaArray)
    {
        if (flag & capabilities.supportedCompositeAlpha)
        {
            compositeAlpha = flag;
            break;
        }
    }


	info.setCompositeAlpha(compositeAlpha);

	info.setImageFormat(useFormat);

	info.setImageExtent({ static_cast<std::uint32_t>(clampSize.x), static_cast<std::uint32_t>(clampSize.y) });

	info.setImageArrayLayers(1);

	info.setImageColorSpace(vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear);

	info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst);

	info.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);

	info.setPresentMode(vk::PresentModeKHR::eFifo);

	info.setClipped(true);

	swapchain_ = device_->createSwapchainKHRUnique(info);

	return useFormat;

}

Eugene::Graphics::~Graphics()
{
	queue_.waitIdle();
	device_->waitIdle();

#ifdef USE_IMGUI

	for (auto& image : imageDatas_)
	{
		image.descriptorSet.release();
		ImGui_ImplVulkan_RemoveTexture(*image.descriptorSet);
	}

	ImGui_ImplVulkan_Shutdown();
	device_->waitIdle();
#endif
}

//vk::UniqueDeviceMemory Eugene::Graphics::CreateMemory(vk::UniqueImage& image) const
//{
//	auto memProps = physicalDevice_.getMemoryProperties();
//	auto memRq = device_->getImageMemoryRequirements(*image);
//	std::uint32_t heapIdx = 0u;
//	std::uint32_t memIdx = 0u;
//
//	// ヒープを探す
//	for (std::uint32_t i = 0; i < memProps.memoryHeapCount; i++)
//	{
//		if (memProps.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal)
//		{
//			heapIdx = i;
//			break;
//		}
//	}
//
//	// メモリータイプを探す
//	for (std::uint32_t i = 0; i < memProps.memoryTypeCount; i++)
//	{
//		if (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
//		{
//			if ((memRq.memoryTypeBits >> i) & 0x1)
//			{
//				if (memProps.memoryTypes[i].heapIndex == heapIdx)
//				{
//					memIdx = i;
//					break;
//				}
//			}
//		}
//	}
//	vk::MemoryAllocateInfo allocateInfo{};
//	allocateInfo.setAllocationSize(memRq.size);
//	allocateInfo.setMemoryTypeIndex(memIdx);
//	return image.getOwner().allocateMemoryUnique(allocateInfo);
//}
//
//vk::UniqueDeviceMemory Eugene::VkGraphics::CreateMemory(vk::Buffer& buffer, bool isDeviceLoacal, bool isHostVisible) const
//{
//	auto memProps = physicalDevice_.getMemoryProperties();
//	auto memRq = device_->getBufferMemoryRequirements(buffer);
//	std::uint32_t heapIdx = 0u;
//	std::uint32_t memIdx = 0u;
//
//	// ヒープを探す
//	for (std::uint32_t i = 0; i < memProps.memoryHeapCount; i++)
//	{
//		if (((memProps.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal) == vk::MemoryHeapFlagBits::eDeviceLocal) == isDeviceLoacal)
//		{
//			heapIdx = i;
//			break;
//		}
//	}
//
//	// メモリータイプを探す
//	for (std::uint32_t i = 0; i < memProps.memoryTypeCount; i++)
//	{
//		auto propFlag = isHostVisible ? (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent) : vk::MemoryPropertyFlagBits::eDeviceLocal;
//		if (memProps.memoryTypes[i].propertyFlags & propFlag)
//		{
//			if ((memRq.memoryTypeBits >> i) & 0x1)
//			{
//				if (memProps.memoryTypes[i].heapIndex == heapIdx)
//				{
//					memIdx = i;
//					break;
//				}
//			}
//		}
//	}
//	vk::MemoryAllocateInfo allocateInfo{};
//	allocateInfo.setAllocationSize(memRq.size);
//	allocateInfo.setMemoryTypeIndex(memIdx);
//	return device_->allocateMemoryUnique(allocateInfo);
//}

#ifdef USE_IMGUI
ImGui_ImplVulkanH_Window* Eugene::Graphics::GetImguiWindow(void)
{
	return &::imguiWindowH;
}

vk::RenderPass Eugene::Graphics::GetRenderPass(void)
{
	return *imguiRenderPass_;
}

vk::Framebuffer Eugene::Graphics::GetFrameBuffer(void)
{
	return *imguiFrameBuffer_[backBufferIdx_];
}
#endif

//Eugene::BufferResource* Eugene::Graphics::CreateReadableBufferResource(std::uint64_t size, bool isUnordered)const
//{
//	return new VkUnloadableBufferResource{ *allocator_, size, isUnordered};
//}
//
//Eugene::BufferResource* Eugene::Graphics::CreateBufferResource(std::uint64_t size, bool isUnordered) const
//{
//	return new VkBufferResource{*allocator_, size, isUnordered};
//}
//
//Eugene::BufferResource* Eugene::Graphics::CreateBufferResource(Image& texture) const
//{
//	return new VkUnloadableBufferResource{ *allocator_, texture};
//}
//
//Eugene::BufferResource* Eugene::Graphics::CreateUnloadableBufferResource(std::uint64_t size) const
//{
//	return new VkUnloadableBufferResource{ *allocator_, size,false };
//}
//
//Eugene::ImageResource* Eugene::Graphics::CreateImageResource(const TextureInfo& formatData) const
//{
//	// 
//	return new VkImageResource{ *allocator_,formatData};
//}
//
//Eugene::ImageResource* Eugene::Graphics::CreateImageResource(
//	const glm::ivec2& size, Format format,
//	std::uint32_t arraySize,
//	std::uint8_t mipLeveles,
//	std::uint8_t sampleCount,
//	std::optional<std::span<float, 4>> clearColor
//	)
//{
//	if (format == Format::AUTO_BACKBUFFER)
//	{
//		format = backBufferFormat_;
//	}
//	return new VkImageResource{ *allocator_, size,format,arraySize,mipLeveles,sampleCount,clearColor};
//}


//Eugene::ShaderResourceViews* Eugene::Graphics::CreateShaderResourceViews(const ArgsSpan<Bind>& viewTypes) const
//{
//	return new VkShaderResourceViews{*device_, viewTypes};
//}
//
//Eugene::DepthStencilViews* Eugene::Graphics::CreateDepthStencilViews(std::uint64_t size) const
//{
//	return new VkDepthStencilView{*device_, static_cast<std::uint32_t>(size)};
//}
//
//Eugene::RenderTargetViews* Eugene::Graphics::CreateRenderTargetViews(std::uint64_t size, bool isShaderVisible) const
//{
//	return new VkRenderTargetViews{*device_,size};
//}



void Eugene::Graphics::Present(void)
{
	if (isMinimized)
	{
		// 最小状態の時はPresent処理しない
		return;
	}

	vk::SwapchainKHR sws[]{ *swapchain_,*swapchain_ };
	vk::PresentInfoKHR info{};
	info.setImageIndices(backBufferIdx_);
	info.setSwapchains(*swapchain_);
	
	vk::Result result{};

	try
	{
		result = queue_.presentKHR(info);
	}
	catch (const std::exception& e)
	{
		DebugClass.Error(e.what());
		throw e;
	}

	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		DebugClass.Error("PresentError");
		return;
	}
	device_->resetFences(*fence_);
	
	
	if (device_->acquireNextImageKHR(*swapchain_, UINT64_MAX, {}, *fence_, &backBufferIdx_) != vk::Result::eSuccess)
	{
		DebugClass.Error("acquireNextImageKHR Error");
		return;
	}

	if (device_->waitForFences(*fence_, true, UINT64_MAX) != vk::Result::eSuccess)
	{
		DebugClass.Error("WaitFence Error");
		return;
	}

	device_->waitIdle();
	device_->resetFences(*fence_);

#ifdef USE_IMGUI
	::imguiWindowH.FrameIndex = backBufferIdx_;
#endif
}

//Eugene::SamplerViews* Eugene::Graphics::CreateSamplerViews(const ArgsSpan<Bind>& viewTypes) const
//{
//	return new VkSamplerViews{*device_, viewTypes};
//}

void Eugene::Graphics::ResizeBackBuffer(const glm::vec2& size,void* window) {
    queue_.waitIdle();
    device_->waitIdle();

    if (size == zeroVector2<float>) {
        // ウィンドウ最小時
        isMinimized = true;
        return;
    } else if (isMinimized) {
        // 最小状態から復帰時
        isMinimized = false;
        return;
    }


    renderTargetViews_.Final();
    for (auto &buffer: buffers_) {
        buffer.Final();
    }
    swapchain_.reset();

#ifdef EUGENE_ANDROID
    surfaceKhr_.reset();
    if (window != nullptr)
    {
        pApp->window = static_cast<ANativeWindow *>(window);
    }
    vk::AndroidSurfaceCreateInfoKHR surfaceInfo{};
    surfaceInfo.setWindow(pApp->window);
    surfaceKhr_ = instance_->createAndroidSurfaceKHRUnique(surfaceInfo);
#endif

    auto swapChainFormat = CreateSwapChain(size);
    CreateBackBuffer(swapChainFormat, size);

#ifdef USE_IMGUI
    CreateImguiFrameBuffer(size);
#endif
    device_->resetFences(*fence_);

    try {
        auto result = device_->acquireNextImageKHR(*swapchain_, UINT64_MAX, {}, *fence_,
                                                   &backBufferIdx_);
        result = device_->waitForFences(*fence_, true, UINT64_MAX);
    }
    catch (std::exception& e)
    {
		DebugClass.Log("ResizeError={}",e.what());
        throw e;
    }
    device_->resetFences(*fence_);

	DebugClass.Log("Resize完了");
}


void Eugene::Graphics::CreateInstance(void)
{
	VULKAN_HPP_DEFAULT_DISPATCHER.init(dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

	auto appInfo{ vk::ApplicationInfo{} };
	appInfo.setPApplicationName("vulkanTest");
	appInfo.setPEngineName("No Engine");
	appInfo.setEngineVersion(VK_MAKE_VERSION(1, 3, 0));
	appInfo.setApiVersion(VK_API_VERSION_1_3);

	const std::vector< const char* > layers{
		"VK_LAYER_KHRONOS_validation"
#ifdef _DEBUG
		//"VK_LAYER_RENDERDOC_Capture"
#endif 
	};
	std::vector<const char*> extens{
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef EUGENE_WINDOWS
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else EUGENE_ANDROID
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#endif

#ifdef _DEBUG
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
		VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
		VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME
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

	VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance_);
}

void Eugene::Graphics::CreateDevice(void)
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
		throw EugeneLibException{"使用できる物理デバイスがありません"};
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
		//VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		//VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
		
#ifdef EUGENE_WINDOWS
		VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME
#endif
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
	VULKAN_HPP_DEFAULT_DISPATCHER.init(*device_);
}

void Eugene::Graphics::SetUpVma(void)
{
    vma::VulkanFunctions vulkanFunc{};

    vulkanFunc.setVkGetInstanceProcAddr(dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));
    vulkanFunc.setVkGetDeviceProcAddr(dl.getProcAddress<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr"));

    vma::AllocatorCreateInfo allocatorInfo{};
    allocatorInfo.setPhysicalDevice(physicalDevice_);
    allocatorInfo.setDevice(*device_);
    allocatorInfo.setInstance(*instance_);
    allocatorInfo.setVulkanApiVersion(VK_API_VERSION_1_3);
    allocatorInfo.setPVulkanFunctions(&vulkanFunc);
    allocatorInfo.setFlags(vma::AllocatorCreateFlagBits::eExtMemoryBudget);
    allocator_ = vma::createAllocatorUnique(allocatorInfo);
}


std::pair<Eugene::GpuMemoryInfo, Eugene::GpuMemoryInfo> Eugene::Graphics::GetGpuMemoryInfo(void) const
{
	auto budgets = allocator_->getHeapBudgets();
	auto properties = physicalDevice_.getMemoryProperties();
	std::pair<Eugene::GpuMemoryInfo, Eugene::GpuMemoryInfo> rtn;
	for (auto i = 0u; i < properties.memoryHeapCount; i++)
	{
		if ((properties.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal))
		{
			rtn.first.usage += budgets[i].usage;
			rtn.first.budget += budgets[i].budget;
		}
		else
		{
			rtn.second.usage += budgets[i].usage;
			rtn.second.budget += budgets[i].budget;
		}
	}
	return rtn;
}



#ifdef USE_IMGUI
void Eugene::Graphics::ImguiNewFrame(void) const
{
	ImGui_ImplVulkan_NewFrame();
}
void* Eugene::Graphics::GetImguiImageID(std::uint64_t index) const
{
	if (index >= imguiImageMax_)
	{
		return nullptr;
	}
	return *imageDatas_[index].descriptorSet;
}

void Eugene::Graphics::SetImguiImage(ImageResource& imageResource, std::uint64_t index)
{
	if (index >= imguiImageMax_)
	{
		return;
	}

	auto& data{ *static_cast<ImageResource::ImageData*>(imageResource.GetResource())};
	auto format = Graphics::FormatToVkFormat[static_cast<size_t>(imageResource.GetFormat())];
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(*data.image_);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(format);
	viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	viewInfo.subresourceRange.setLevelCount(data.mipmapLevels_);
	viewInfo.subresourceRange.setLayerCount(data.arraySize_);
	imageDatas_[index].imageView = device_->createImageViewUnique(viewInfo);
	if (imageDatas_[index].descriptorSet)
	{
		vk::DescriptorImageInfo imgInfo{};
		imgInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
		imgInfo.setSampler(*imguiSampler_);
		imgInfo.setImageView(*imageDatas_[index].imageView);
		vk::WriteDescriptorSet write[1] = {};
		write[0].dstSet = *imageDatas_[index].descriptorSet;
		write[0].descriptorCount = 1;
		write[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		write[0].setImageInfo(imgInfo);
		device_->updateDescriptorSets(write,nullptr);
	}
	else
	{
		auto set = vk::DescriptorSet{ ImGui_ImplVulkan_AddTexture(*imguiSampler_, *imageDatas_[index].imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) };
		imageDatas_[index].descriptorSet = vk::UniqueDescriptorSet{ 
			set,
		};
	}
}

void Eugene::Graphics::InitImgui(vk::Format useVkformat, const uint32_t& bufferNum, const glm::vec2& size)
{
	ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void* data) {
		return reinterpret_cast<vk::DynamicLoader*>(data)->getProcAddress<PFN_vkVoidFunction>(function_name);
		}, &dl);

	// imgui用ディスクリプタープールの生成
	{
		vk::DescriptorPoolCreateInfo dPoolInfo{};
		vk::DescriptorPoolSize a;
		constexpr vk::DescriptorPoolSize  sizes[]{
			vk::DescriptorPoolSize{ vk::DescriptorType::eSampler,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eSampledImage,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eStorageImage,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eUniformTexelBuffer,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eStorageTexelBuffer,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBufferDynamic,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBufferDynamic,1000 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eInputAttachment,1000 }
		};
		dPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
		dPoolInfo.setPoolSizes(sizes);
		dPoolInfo.setMaxSets(1000u * std::size(sizes));
		imguiDescriptorPool_ = device_->createDescriptorPoolUnique(dPoolInfo);
	}

	// imgui用のレンダーパスを生成
	{
		vk::AttachmentDescription colorAttachment{};
		colorAttachment.setFormat(useVkformat);
		colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
		colorAttachment.setLoadOp(vk::AttachmentLoadOp::eLoad);
		colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
		colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
		colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
		colorAttachment.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal);
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
		imguiRenderPass_ = device_->createRenderPassUnique(renderPassInfo);
	}

	ImGui_ImplVulkan_InitInfo info{};
	info.Instance = *instance_;
	info.PhysicalDevice = physicalDevice_;
	info.Device = *device_;
	info.QueueFamily = graphicFamilly_;
	info.PipelineCache = VK_NULL_HANDLE;
	info.DescriptorPool = *imguiDescriptorPool_;
	info.Queue = queue_;
	info.Subpass = 0;
	info.MinImageCount = bufferNum;
	info.ImageCount = bufferNum;
	info.Allocator = callbacks;
	info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	info.CheckVkResultFn = CheckVkResult;
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

	platform_io.Platform_CreateVkSurface = ImGui_ImplVulkan_CreateVkSurface;
	if (!ImGui_ImplVulkan_Init(&info, *imguiRenderPass_))
	{
		throw EugeneLibException{ "Imgui初期化エラー" };
	}

	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.setQueueFamilyIndex(graphicFamilly_);
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	auto tmpCommandPool = device_->createCommandPoolUnique(poolInfo);

	vk::CommandBufferAllocateInfo bufferInfo{};
	bufferInfo.setCommandPool(*tmpCommandPool);
	bufferInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	bufferInfo.setCommandBufferCount(1);
	auto tmpCommandBuffer = std::move(device_->allocateCommandBuffersUnique(bufferInfo)[0]);

	tmpCommandBuffer->reset();
	tmpCommandBuffer->begin(vk::CommandBufferBeginInfo{});
	ImGui_ImplVulkan_CreateFontsTexture(*tmpCommandBuffer);
	tmpCommandBuffer->end();
	vk::SubmitInfo submitInfo{};
	submitInfo.setCommandBuffers(*tmpCommandBuffer);
	queue_.submit(submitInfo);
	queue_.waitIdle();
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	CreateImguiFrameBuffer(size);

	imageDatas_.resize(imguiImageMax_);
	vk::SamplerCreateInfo samplerInfo{};
	samplerInfo.setMagFilter(vk::Filter::eLinear);
	samplerInfo.setMinFilter(vk::Filter::eLinear);
	samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
	samplerInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
	samplerInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
	samplerInfo.setMinLod(-1000);
	samplerInfo.setMaxLod(1000);
	samplerInfo.setMaxAnisotropy(1.0f);
	imguiSampler_ = device_->createSamplerUnique(samplerInfo);

}

void Eugene::Graphics::CreateImguiFrameBuffer(const glm::vec2& size)
{
	imguiFrameBuffer_.resize(buffers_.size());
	auto& backViews = *static_cast<std::vector<RenderTargetViews::Data>*>(renderTargetViews_.GetViews());
	for (auto i = 0ull; i < imguiFrameBuffer_.size(); i++)
	{
		vk::ImageView attachments[] = {
			*backViews[i].imageView
		};
		vk::FramebufferCreateInfo info{};
		info.setRenderPass(*imguiRenderPass_);
		info.setAttachmentCount(1);
		info.setAttachments(attachments);
		info.setWidth(static_cast<std::uint32_t>(size.x));
		info.setHeight(static_cast<std::uint32_t>(size.y));
		info.setLayers(1);

		imguiFrameBuffer_[i] = device_->createFramebufferUnique(info);
	}
}

#endif 
