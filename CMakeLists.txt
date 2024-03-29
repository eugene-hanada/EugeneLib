cmake_minimum_required (VERSION 3.20)
project(EugeneLib)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

option(USE_WINDOWS "Windowsm向けにビルドするか？" ON)
option(USE_IMGUI "Imguiを使用するか?" OFF)
option(USE_VULKAN "Vulkanを使用するか?" OFF)
option(USE_DX12 "DirectX12を使用するか？" ON)
option(USE_EFFEKSEER "Effekseerを使用するか?" OFF)
option(BUILD_SAMPLE "サンプルをbuildするか？" OFF)
option(BUILD_DOCTEST "doctestを使ったテストをビルドするか？" OFF)

if(USE_WINDOWS)
	# Windows向けビルド時
	if (NOT USE_VULKAN)
		# Vulkanを使用しない時は一応DirectX12のオプションをONに
		set(USE_DX12 ON)
	endif()
endif()

set(LIBRARY_OUTPUT_PATH ../)

# システム系のクラスのソースファイルをセット
set(SystemSource 
	Source/System/System.cpp
	Source/System/GamePad.cpp
	Source/System/DynamicLibrary.cpp
)

if(USE_IMGUI)

	# imgui使用時
	set(SystemSource 
		${SystemSource}
		Include/ThirdParty/imgui/imgui.cpp
		Include/ThirdParty/imgui/imgui_demo.cpp
		Include/ThirdParty/imgui/imgui_draw.cpp
		Include/ThirdParty/imgui/imgui_tables.cpp
		Include/ThirdParty/imgui/imgui_widgets.cpp
		ImGuizmo/GraphEditor.cpp
		ImGuizmo/ImCurveEdit.cpp
		ImGuizmo/ImGradient.cpp
		ImGuizmo/ImGuizmo.cpp
		ImGuizmo/ImSequencer.cpp
	)
endif()

# グラフィック系クラスのソースファイルをセット
set(GraphicsSource
	Source/Graphics/Graphics.cpp
	Source/Graphics/GpuEngine.cpp
	Source/Graphics/CommandList.cpp
	Source/Graphics/GpuResource.cpp
	Source/Graphics/BufferResource.cpp
	Source/Graphics/ImageResource.cpp
	Source/Graphics/GraphicsPipeline.cpp
	Source/Graphics/RenderTargetViews.cpp
	Source/Graphics/DepthStencilViews.cpp
	Source/Graphics/ShaderResourceViews.cpp
	Source/Graphics/VertexView.cpp
	Source/Graphics/IndexView.cpp
	Source/Graphics/Shader.cpp
	Source/Graphics/Image.cpp
	Source/Graphics/Sampler.cpp
	Source/Graphics/SamplerViews.cpp
	Source/Graphics/ResourceBindLayout.cpp)

# Common系のソースファイルをセット
set(CommonSource
	Source/Common/Debug.cpp
	Source/Common/EugeneLibException.cpp
)

# Math系のソースファイルをセット
set(MathSource
	Source/Math/Geometry.cpp
	Source/Math/Random.cpp)

# サウンド系のクラスのソースファイルをセット
set(SoundSource
	Source/Sound/Sound.cpp
	Source/Sound/SoundBase.cpp
	Source/Sound/SoundSpeaker.cpp
	Source/Sound/SoundStreamSpeaker.cpp
	Source/Sound/SoundControl.cpp
	Source/Sound/Sound3DControl.cpp
	Source/Sound/SoundFile.cpp
)


if (USE_WINDOWS)
	# Windows用の時

	# システム系の実装クラスのソースファイルをセット
	set(SystemApiSource 
		Source/System/Windows/WindowsSystem.cpp
		Source/System/Windows/Dll.cpp)

	if (USE_IMGUI)
		# imgui使用時
		set(SystemApiSource 
			${SystemApiSource}
			 Include/ThirdParty/imgui/backends/imgui_impl_win32.cpp)
	endif()

	# サウンド系の実装クラスのソースファイルをセット
	set(SoundApiSource
		Source/Sound/Xaudio2/Xa2Sound.cpp
		Source/Sound/Xaudio2/Xa2SoundSpeaker.cpp
		Source/Sound/Xaudio2/Xa2SoundStreamSpeaker.cpp
		Source/Sound/Xaudio2/Xa2SoundControl.cpp
		Source/Sound/Xaudio2/Xa2Sound3DControl.cpp
	)
endif()

if(USE_DX12 AND NOT USE_VULKAN)
	# グラフィック系の実装クラスのソースファイルをセット
	set(GraphicsApiSource
		Source/Graphics/DirectX12/Dx12CommandList.cpp
		Source/Graphics/DirectX12/Dx12DepthStencilViews.cpp
		Source/Graphics/DirectX12/Dx12GpuEngine.cpp
		Source/Graphics/DirectX12/Dx12BufferResource.cpp
		Source/Graphics/DirectX12/Dx12ImageResource.cpp
		Source/Graphics/DirectX12/Dx12Graphics.cpp
		Source/Graphics/DirectX12/Dx12GraphicsPipeline.cpp
		Source/Graphics/DirectX12/Dx12RenderTargetViews.cpp
		Source/Graphics/DirectX12/Dx12ShaderResourceViews.cpp
		Source/Graphics/DirectX12/Dx12VertexView.cpp
		Source/Graphics/DirectX12/Dx12IndexView.cpp
		Source/Graphics/DirectX12/Dx12Views.cpp
		Source/Graphics/DirectX12/Dx12Sampler.cpp
		Source/Graphics/DirectX12/Dx12SamplerViews.cpp
		Source/Graphics/DirectX12/Dx12ResourceBindLayout.cpp)

	if (USE_IMGUI)
		set(GraphicsApiSource
		${GraphicsApiSource}
		Include/ThirdParty/imgui/backends/imgui_impl_dx12.cpp
		)
	endif()
elseif(USE_VULKAN)
	# Vulkan使用時
	set(GraphicsApiSource
		Source/Graphics/Vulkan/VkGraphics.cpp
		Source/Graphics/Vulkan/VkGpuEngine.cpp
		Source/Graphics/Vulkan/VkImageResource.cpp
		Source/Graphics/Vulkan/VkResourceBindLayout.cpp
		Source/Graphics/Vulkan/VkCommandList.cpp
		Source/Graphics/Vulkan/VkDepthStencilViews.cpp
		Source/Graphics/Vulkan/VkGraphicsPipeline.cpp
		Source/Graphics/Vulkan/VkBufferResource.cpp
		Source/Graphics/Vulkan/VkVertexView.cpp
		Source/Graphics/Vulkan/VkShaderResourceViews.cpp
		Source/Graphics/Vulkan/VkSampler.cpp
		Source/Graphics/Vulkan/VkSamplerViews.cpp
		Source/Graphics/Vulkan/VkRenderTargetViews.cpp
		Source/Graphics/Vulkan/VkIndexView.cpp)

	if (USE_IMGUI)
	set(GraphicsApiSource
	${GraphicsApiSource}
	Include/ThirdParty/imgui/backends/imgui_impl_vulkan.cpp
	)
	endif()
endif()

# ランタイムライブラリを静的にリンクする
if(MSVC)
	# visual stdio用にunicodeを使うように
	add_definitions(-D_UNICODE -DUNICODE)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ライブラリを作る
add_library(EugeneLib STATIC  
	${SystemSource}
	 ${SystemApiSource}
	${GraphicsSource}
	${GraphicsApiSource}
	${CommonSource}
	${MathSource}
	${SoundSource}
	${SoundApiSource}
)

if (USE_WINDOWS)
	target_compile_definitions(EugeneLib PRIVATE USE_WINDOWS)
endif()

if (USE_DX12 AND NOT USE_VULKAN)
	# DirectX12使用時defineを用意
	target_compile_definitions(EugeneLib PRIVATE USE_DX12)
elseif(USE_VULKAN)
	# Vulkan使用時define用意
	target_compile_definitions(EugeneLib PRIVATE USE_VULKAN)
	target_include_directories(EugeneLib PRIVATE $ENV{VULKAN_SDK}/Include)

	if (NOT USE_EFFEKSEER)
	target_link_libraries(EugeneLib PRIVATE $ENV{VULKAN_SDK}/Lib/VkLayer_utils.lib)
	target_link_libraries(EugeneLib PRIVATE $ENV{VULKAN_SDK}/Lib/vulkan-1.lib)
	endif()
endif()

if (USE_IMGUI)
	target_compile_definitions(EugeneLib PUBLIC USE_IMGUI)
	target_include_directories(EugeneLib PUBLIC Include/ThirdParty/imgui)
	target_include_directories(EugeneLib PUBLIC ./ImGuizmo)
endif()



 target_compile_definitions(EugeneLib PUBLIC NOMINMAX)
 target_compile_definitions(EugeneLib PUBLIC GLM_FORCE_LEFT_HANDED)
 target_compile_definitions(EugeneLib PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)

 # サンプルをビルドするか？
 if (BUILD_SAMPLE)
 add_subdirectory("Sample")
 endif()


 # Effekseerを使用するか？
 if (USE_EFFEKSEER)

	# 使用時
	add_subdirectory("Effekseer")
	target_link_libraries(EugeneLib LLGI Effekseer)
	target_compile_definitions(EugeneLib PUBLIC USE_EFFEKSEER)
	target_include_directories(EugeneLib PUBLIC "Effekseer/Dev/Cpp/Effekseer")

	
	# Vulkan使用するか？
	if (USE_VULKAN)
		# VULKAN使用時
		target_link_libraries(EugeneLib EffekseerRendererVulkan glslang SPIRV)
		target_include_directories(EugeneLib PUBLIC "Effekseer/Dev/Cpp/EffekseerRendererVulkan")
	else()
		# 
		target_link_libraries(EugeneLib EffekseerRendererDX12)
		target_include_directories(EugeneLib PUBLIC "Effekseer/Dev/Cpp/EffekseerRendererDX12")
	endif()

  else()

 endif()

 if (BUILD_DOCTEST)
 add_subdirectory("Test")
 endif()