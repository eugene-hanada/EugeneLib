cmake_minimum_required (VERSION 3.20)
project(EugeneLib)

# c++23を使うのでセット
set(CMAKE_CXX_STANDARD 23)

option(EUGENE_USE_WINDOWS "Windowsm向けにビルドするか？" ON)
option(EUGENE_USE_ANDROID "ANDROID向けにビルドするか？" OFF)
option(EUGENE_USE_IMGUI "Imguiを使用するか?" OFF)
option(EUGENE_USE_VULKAN "Vulkanを使用するか?" OFF)
option(EUGENE_USE_DX12 "DirectX12を使用するか？" ON)
option(EUGENE_USE_EFFEKSEER "Effekseerを使用するか?" OFF)
option(EUGENE_BUILD_SAMPLE "サンプルをbuildするか？" OFF)
option(EUGENE_BUILD_DOCTEST "doctestを使ったテストをビルドするか？" OFF)
option(EUGENE_BUILD_SOUND "サウンド機能をビルドするか？" ON)

if (EUGENE_USE_EFFEKSEER)
	set(EUGENE_EFFEKSEER_DIR "" CACHE PATH "エフェクシアビルド済みファイルのディレクトリ")
endif ()

if(EUGENE_USE_WINDOWS)
	# Windows向けビルド時
	if (NOT EUGENE_USE_VULKAN)
		# Vulkanを使用しない時は一応DirectX12のオプションをONに
		set(EUGENE_USE_DX12 ON)
	endif()
elseif(EUGENE_USE_ANDROID)
	set(EUGENE_USE_VULKAN ON)
	set(EUGENE_USE_DX12 OFF)
endif()

set(LIBRARY_OUTPUT_PATH ../)

# システム系のクラスのソースファイルをセット
set(SystemSource 
	Source/System/System.cpp
	Source/System/GamePad.cpp
	Source/System/DynamicLibrary.cpp
)

if(EUGENE_USE_IMGUI)

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

# Utils系のソースファイルをセット
set(UtilsSource
	Source/Utils/EugeneLibException.cpp
	Source/Debug/Debug.cpp
)

# Math系のソースファイルをセット
set(MathSource
	Source/Math/Random.cpp 
	Source/Math/Math.cpp
)

# サウンド系のクラスのソースファイルをセット
set(SoundSource
	Source/Sound/Sound.cpp
	Source/Sound/SoundBase.cpp
	Source/Sound/SoundSpeaker.cpp
	Source/Sound/SoundStreamSpeaker.cpp
	Source/Sound/SoundControl.cpp
	Source/Sound/Sound3DControl.cpp
	Source/Sound/SoundFile.cpp
	Source/Sound/SoundStreamFile.cpp
)



if (EUGENE_USE_WINDOWS)
	# Windows用の時

	# システム系の実装クラスのソースファイルをセット
	set(SystemApiSource 
		Source/System/Windows/WindowsSystem.cpp
		Source/System/Windows/Dll.cpp)

	if (EUGENE_USE_IMGUI)
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
elseif(EUGENE_USE_ANDROID)
	set(SystemApiSource
		Source/System/Android/AndroidSystem.cpp
	)

	set(
		SoundApiSource
		Source/Sound/AAudio/AaSound.cpp
		Source/Sound/AAudio/AaSoundSpeaker.cpp
		Source/Sound/AAudio/AaSoundStreamSpeaker.cpp
		Source/Sound/AAudio/AaSoundControl.cpp
		Source/Sound/AAudio/AaSound3DControl.cpp
		Source/Sound/AAudio/AaSubmix.cpp
		Source/Sound/AAudio/AaVoice.cpp
		Source/Sound/AAudio/AaSourceVoice.cpp
		Source/Sound/AAudio/AaMaster.cpp
		Source/Sound/AAudio/resampler/IntegerRatio.cpp
		Source/Sound/AAudio/resampler/LinearResampler.cpp
		Source/Sound/AAudio/resampler/MultiChannelResampler.cpp
		Source/Sound/AAudio/resampler/PolyphaseResampler.cpp
		Source/Sound/AAudio/resampler/PolyphaseResamplerMono.cpp
		Source/Sound/AAudio/resampler/PolyphaseResamplerStereo.cpp
		Source/Sound/AAudio/resampler/SincResampler.cpp
		Source/Sound/AAudio/resampler/SincResamplerStereo.cpp
	)

endif()

if(EUGENE_USE_DX12 AND NOT EUGENE_USE_VULKAN)
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
		Source/Graphics/DirectX12/Dx12ResourceBindLayout.cpp
		Include/ThirdParty/D3D12MemoryAllocator/src/D3D12MemAlloc.cpp
	)

	if (EUGENE_USE_IMGUI)
		set(GraphicsApiSource
		${GraphicsApiSource}
		Include/ThirdParty/imgui/backends/imgui_impl_dx12.cpp
		)
	endif()
elseif(EUGENE_USE_VULKAN)
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

	if (EUGENE_USE_IMGUI)
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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# ライブラリを作る
add_library(EugeneLib STATIC  
	${SystemSource}
	 ${SystemApiSource}
	${GraphicsSource}
	${GraphicsApiSource}
	${UtilsSource}
	${MathSource}
	${SoundSource}
	${SoundApiSource}
)

if (EUGENE_BUILD_SOUND)
	target_compile_definitions(EugeneLib PUBLIC EUGENE_SOUND)
endif()

if (EUGENE_USE_WINDOWS)
	target_compile_definitions(EugeneLib PUBLIC EUGENE_WINDOWS)

	if (EUGENE_BUILD_SOUND)
		target_compile_definitions(EugeneLib PUBLIC EUGENE_XAUDIO)
	endif()
elseif(EUGENE_USE_ANDROID)
	target_compile_definitions(EugeneLib PUBLIC EUGENE_ANDROID)

	if (EUGENE_BUILD_SOUND)
		target_compile_definitions(EugeneLib PUBLIC EUGENE_AAUDIO)
	endif()

	find_package(game-activity REQUIRED CONFIG)
	target_link_libraries(EugeneLib PUBLIC android log game-activity::game-activity aaudio)
endif()

if (EUGENE_USE_DX12 AND NOT EUGENE_USE_VULKAN)
	# DirectX12使用時defineを用意
	target_compile_definitions(EugeneLib PRIVATE USE_DX12)
	target_include_directories(EugeneLib PRIVATE Include/ThirdParty/D3D12MemoryAllocator/include)
elseif(EUGENE_USE_VULKAN)
	# Vulkan使用時define用意
	target_compile_definitions(EugeneLib PRIVATE USE_VULKAN)
	target_include_directories(EugeneLib PRIVATE ./Include/ThirdParty/VulkanMemoryAllocator-Hpp/Vulkan-Hpp)
	target_include_directories(EugeneLib PRIVATE ./Include/ThirdParty/VulkanMemoryAllocator-Hpp/Vulkan-Hpp/Vulkan-Headers/include)
	target_include_directories(EugeneLib PRIVATE ./Include/ThirdParty/VulkanMemoryAllocator-Hpp/VulkanMemoryAllocator/include)

	target_compile_definitions(EugeneLib PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
	target_compile_definitions(EugeneLib PRIVATE VMA_STATIC_VULKAN_FUNCTIONS=0)
	 
	if (EUGENE_USE_IMGUI)
		target_compile_definitions(EugeneLib PRIVATE IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
	endif()


	if (NOT USE_EFFEKSEER)
		if (NOT EUGENE_USE_ANDROID)
			target_link_libraries(EugeneLib PRIVATE $ENV{VULKAN_SDK}/Lib/VkLayer_utils.lib)
		endif()
	endif()
endif()

if (EUGENE_USE_IMGUI)
	target_compile_definitions(EugeneLib PUBLIC USE_IMGUI)
	target_include_directories(EugeneLib PUBLIC Include/ThirdParty/imgui)
	target_include_directories(EugeneLib PUBLIC ./ImGuizmo)
endif()



 target_compile_definitions(EugeneLib PUBLIC NOMINMAX)
 target_compile_definitions(EugeneLib PUBLIC GLM_FORCE_LEFT_HANDED)
 target_compile_definitions(EugeneLib PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)

 add_subdirectory("Include/ThirdParty/fmt")
 target_link_libraries(EugeneLib PRIVATE fmt::fmt)

 add_subdirectory("Include/ThirdParty/mimalloc")
 target_link_libraries(EugeneLib PUBLIC mimalloc-static)
 target_include_directories(EugeneLib PUBLIC "Include/ThirdParty/mimalloc/include")

 # サンプルをビルドするか？
 if (EUGENE_BUILD_SAMPLE)
 add_subdirectory("Sample")
 endif()


 # Effekseerを使用するか？
 if (EUGENE_USE_EFFEKSEER)

	# 使用時
	target_include_directories(EugeneLib PUBLIC ${EUGENE_EFFEKSEER_DIR}/include/Effekseer)
	target_compile_definitions(EugeneLib PUBLIC USE_EFFEKSEER)
	if (EUGENE_USE_ANDROID)
		message("対応していない")
	else ()
		target_link_libraries(EugeneLib PUBLIC ${EUGENE_EFFEKSEER_DIR}/lib/LLGId.lib ${EUGENE_EFFEKSEER_DIR}/lib/Effekseerd.lib)


		# Vulkan使用するか？
		if (EUGENE_USE_VULKAN)
			# VULKAN使用時
			target_link_libraries(EugeneLib PUBLIC ${EUGENE_EFFEKSEER_DIR}/lib/EffekseerRendererVulkand.lib $ENV{VULKAN_SDK}/Lib/vulkan-1.lib)
			target_include_directories(EugeneLib PUBLIC ${EUGENE_EFFEKSEER_DIR}/include/EffekseerRendererVulkan)
		else()
			#
			target_link_libraries(EugeneLib PUBLIC ${EUGENE_EFFEKSEER_DIR}/lib/EffekseerRendererDX12d.lib)
			target_include_directories(EugeneLib PUBLIC ${EUGENE_EFFEKSEER_DIR}/include/EffekseerRendererDX12)
		endif()

	endif()
 endif()

 if (EUGENE_BUILD_DOCTEST)
 add_subdirectory("Test")
 target_compile_definitions(EugeneLib PRIVATE BUILD_DOCTEST)
 endif()