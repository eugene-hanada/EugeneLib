#pragma once

#ifdef EUGENE_DX12
#include "DirectX12/Dx12RenderTargetViews.h"
#elif EUGENE_VULKAN
#include "Vulkan/VkRenderTargetViews.h"
#endif