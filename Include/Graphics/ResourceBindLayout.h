#pragma once
#ifdef EUGENE_DX12
#include "DirectX12/Dx12ResourceBindLayout.h"
#elif EUGENE_VULKAN
#include "Vulkan/VkResourceBindLayout.h"
#endif