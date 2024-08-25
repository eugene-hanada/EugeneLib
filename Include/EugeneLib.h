#pragma once


// システム系
#include "System/System.h"

// グラフィック系
#include "Graphics/Graphics.h"

#include "Graphics/GpuResource.h"
#include "Graphics/BufferResource.h"
#include "Graphics/ImageResource.h"

#include "Graphics/Shader.h"
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/ResourceBindLayout.h"
#include "Graphics/RenderTargetViews.h"
#include "Graphics/VertexView.h"
#include "Graphics/IndexView.h"
#include "Graphics/ShaderResourceViews.h"
#include "Graphics/DepthStencilViews.h"
#include "Graphics/Image.h"
#include "Graphics/SamplerViews.h"
#include "Graphics/Sampler.h"

// サウンド系
#ifdef EUGENE_SOUND
#include "Sound/Sound.h"
#include "Sound/SoundSpeaker.h"
#include "Sound/SoundFile.h"
#include "Sound/SoundControl.h"
#include "Sound/Sound3DControl.h"
#include "Sound/SoundStreamSpeaker.h"
#endif

#include "Math/Math.h"
#include "Math/Random.h"

#include "Utils/RingBuffer.h"
#include "Utils/SharedPtr.h"
#include "Utils/Hash.h"

#include "Debug/Debug.h"


