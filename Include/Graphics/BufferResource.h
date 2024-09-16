#pragma once

#ifdef EUGENE_DX12
#include "DirectX12/Dx12BufferResource.h"
#elif EUGENE_VULKAN
#include "Vulkan/VkBufferResource.h"
#endif

