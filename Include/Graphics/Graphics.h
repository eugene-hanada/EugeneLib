#pragma once
#include <optional>
#include "GraphicsPipeline.h"
#include "GraphicsCommon.h"
#include "../ThirdParty/glm/glm/vec2.hpp"
#include "../ThirdParty/glm/glm/vec3.hpp"
#include "Sampler.h"
#include "../Utils/ArgsSpan.h"

#ifdef EUGENE_DX12
#include "DirectX12/Dx12Graphics.h"
#elif EUGENE_VULKAN
#include "Vulkan/VkGraphics.h"
#endif
