#pragma once
//#include "GpuResource.h"
//#include "GraphicsCommon.h"
//#include "../ThirdParty/glm/glm/vec2.hpp"

#ifdef EUGENE_DX12
#include "DirectX12/Dx12ImageResource.h"
#elif EUGENE_VULKAN
#include "Vulkan/VkImageResource.h"
#endif

