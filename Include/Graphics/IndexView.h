#pragma once
#ifdef EUGENE_DX12
#include "DirectX12/Dx12IndexView.h"
#elif EUGENE_VULKAN
#include "Vulkan/VkIndexView.h"
#endif