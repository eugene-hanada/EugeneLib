cmake_minimum_required (VERSION 3.20)
project(EugeneLib)

# c++23を使うのでセット
set(CMAKE_CXX_STANDARD 23)

option(EUGENE_USE_WINDOWS "Windowsm向けにビルドするか？" ON)
option(EUGENE_USE_ANDROID "ANDROID向けにビルドするか？" OFF)
option(EUGENE_USE_IMGUI "Imguiを使用するか?" OFF)
option(EUGENE_USE_VULKAN "Vulkanを使用するか?" OFF)
option(EUGENE_USE_DX12 "DirectX12を使用するか？" ON)
option(EUGENE_BUILD_SAMPLE "サンプルをbuildするか？" OFF)
option(EUGENE_BUILD_DOCTEST "doctestを使ったテストをビルドするか？" OFF)
option(EUGENE_BUILD_SOUND "サウンド機能をビルドするか？" ON)
option(EUGENE_BUILD_NETWORK "ネットワーク通信関連の機能をビルドするか？" ON)

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

# システム系のクラスのソースファイルをセット
set(SystemSource 
	Source/System/System.cpp
	Source/System/GamePad.cpp
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
	Source/Graphics/Shader.cpp
	Source/Graphics/Image.cpp
	)

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

if (EUGENE_BUILD_SOUND)
	# サウンド系のクラスのソースファイルをセット
	set(SoundSource
		Source/Sound/SoundBase.cpp
		Source/Sound/SoundFile.cpp
		Source/Sound/SoundStreamFile.cpp
	)
endif()

if (EUGENE_BUILD_NETWORK)
	set(SystemSource Source/System/NetSocket.cpp)
endif()


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

	if (EUGENE_BUILD_SOUND)
		# サウンド系の実装クラスのソースファイルをセット
		set(SoundApiSource
			Source/Sound/Xaudio2/Xa2Sound.cpp
			Source/Sound/Xaudio2/Xa2SoundSpeaker.cpp
			Source/Sound/Xaudio2/Xa2SoundStreamSpeaker.cpp
			Source/Sound/Xaudio2/Xa2SoundControl.cpp
		)
	endif()

elseif(EUGENE_USE_ANDROID)
	set(SystemApiSource
		Source/System/Android/AndroidSystem.cpp
		Source/System/Linux/SharedObject.cpp
	)

	if (EUGENE_BUILD_SOUND)
		set(
			SoundApiSource
			Source/Sound/AAudio/AaSound.cpp
			Source/Sound/AAudio/AaSoundSpeaker.cpp
			Source/Sound/AAudio/AaSoundStreamSpeaker.cpp
			Source/Sound/AAudio/AaSoundControl.cpp
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

if (EUGENE_BUILD_NETWORK)
	target_compile_definitions(EugeneLib PUBLIC EUGENE_NETWORK)
endif()

if (EUGENE_USE_WINDOWS)
	target_compile_definitions(EugeneLib PUBLIC EUGENE_WINDOWS)

	if (EUGENE_BUILD_SOUND)
		target_compile_definitions(EugeneLib PUBLIC EUGENE_XAUDIO)
	endif()
elseif(EUGENE_USE_ANDROID)
	target_compile_definitions(EugeneLib PUBLIC EUGENE_ANDROID)
	target_include_directories(EugeneLib PUBLIC ${EUGENE_AGDK_PATH}/include)
	if (EUGENE_BUILD_SOUND)
		target_compile_definitions(EugeneLib PUBLIC EUGENE_AAUDIO)
	endif()

	find_package(game-activity REQUIRED CONFIG)
	target_link_libraries(EugeneLib PUBLIC android log aaudio)
endif()

if (EUGENE_USE_DX12)
	# DirectX12使用時defineを用意
	target_compile_definitions(EugeneLib PUBLIC EUGENE_DX12)
	target_include_directories(EugeneLib PRIVATE Include/ThirdParty/D3D12MemoryAllocator/include)
elseif(EUGENE_USE_VULKAN)
	# Vulkan使用時define用意
	target_compile_definitions(EugeneLib PUBLIC EUGENE_VULKAN)
	target_include_directories(EugeneLib PUBLIC ./Include/ThirdParty/VulkanMemoryAllocator-Hpp/Vulkan-Hpp/Vulkan-Headers/include)
	target_include_directories(EugeneLib PUBLIC ./Include/ThirdParty/VulkanMemoryAllocator-Hpp/VulkanMemoryAllocator/include)
	target_include_directories(EugeneLib PUBLIC ./Include/ThirdParty/VulkanMemoryAllocator-Hpp/include)

	target_compile_definitions(EugeneLib PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VMA_STATIC_VULKAN_FUNCTIONS=0)
	if (EUGENE_USE_IMGUI)
		target_compile_definitions(EugeneLib PRIVATE IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
	endif()

endif()

if (EUGENE_USE_IMGUI)
	target_compile_definitions(EugeneLib PUBLIC EUGENE_IMGUI)
	target_include_directories(EugeneLib PUBLIC Include/ThirdParty/imgui)
	target_include_directories(EugeneLib PUBLIC ./ImGuizmo)
endif()



 target_compile_definitions(EugeneLib PUBLIC NOMINMAX GLM_FORCE_LEFT_HANDED GLM_FORCE_DEPTH_ZERO_TO_ONE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_CONFIGURATION_TYPES STREQUAL "Debug")
	target_compile_definitions(EugeneLib PUBLIC EUGENE_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_CONFIGURATION_TYPES STREQUAL "Release")
	target_compile_definitions(EugeneLib PUBLIC EUGENE_RELEASE)
endif()

 


 add_subdirectory("Include/ThirdParty/fmt")
 target_link_libraries(EugeneLib PRIVATE fmt::fmt)

 add_subdirectory("Include/ThirdParty/mimalloc")
 target_link_libraries(EugeneLib PUBLIC mimalloc-static)
 target_include_directories(EugeneLib PUBLIC "Include/ThirdParty/mimalloc/include")

 # サンプルをビルドするか？
 if (EUGENE_BUILD_SAMPLE AND NOT EUGENE_USE_ANDROID)
	add_subdirectory("Sample")
 endif()

 if (EUGENE_BUILD_DOCTEST)
 add_subdirectory("Test")
	target_compile_definitions(EugeneLib PRIVATE BUILD_DOCTEST)
 endif()