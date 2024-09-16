#pragma once
#ifdef EUGENE_DX12
#include "DirectX12/Dx12GraphicsPipeline.h"
#elif EUGENE_VULKAN
#include "Vulkan/VkGraphicsPipeline.h"
#endif
