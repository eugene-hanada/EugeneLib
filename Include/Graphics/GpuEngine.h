#pragma once

#ifdef EUGENE_DX12
#include "DirectX12/Dx12Graphics.h"
#elif EUGENE_VULKAN
#include "Vulkan/VkGpuEngine.h"
#endif

