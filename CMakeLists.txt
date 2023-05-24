cmake_minimum_required (VERSION 3.20)
project(EugeneLib)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

option(USE_IMGUI "Imguiを使用するか?" OFF)
option(USE_VULKAN "Vulkanを使用するか?" OFF)
option(USE_EFFEKSEER "Effekseerを使用するか?" OFF)
option(BUILD_SAMPLE "サンプルをbuildするか？" OFF)

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
)

# Common系のソースファイルをセット
set(CommonSource
	Source/Common/Debug.cpp
	Source/Common/EugeneLibException.cpp
)

# Math系のソースファイルをセット
set(MathSource
	Source/Math/Geometry.cpp
	Include/ThirdParty/DirectXMath/DirectXCollision.h
	Include/ThirdParty/DirectXMath/DirectXCollision.inl
	Include/ThirdParty/DirectXMath/DirectXColors.h
	Include/ThirdParty/DirectXMath/DirectXMath.h
	Include/ThirdParty/DirectXMath/DirectXMathConvert.inl
	Include/ThirdParty/DirectXMath/DirectXMathMatrix.inl
	Include/ThirdParty/DirectXMath/DirectXMathMisc.inl
	Include/ThirdParty/DirectXMath/DirectXMathVector.inl
	Include/ThirdParty/DirectXMath/DirectXPackedVector.h
	Include/ThirdParty/DirectXMath/DirectXPackedVector.inl
	Source/Math/Vector3.cpp
	Source/Math/Quaternion.cpp
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
	Source/Sound/Wave.cpp
	Source/Sound/OggVorbis.cpp
)


if (MSVC)

	# システム系の実装クラスのソースファイルをセット
	set(SystemApiSource 
		Source/System/Windows/WindowsSystem.cpp
		Source/System/Windows/Dll.cpp
		)

	if (USE_IMGUI)
		# imgui使用時
		set(SystemApiSource 
			${SystemApiSource}
			 Include/ThirdParty/imgui/backends/imgui_impl_win32.cpp
		)
	endif()

	if (USE_VULKAN)
		# Vulkan使用時
	else()
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
		)

		if (USE_IMGUI)
			  set(GraphicsApiSource
			  ${GraphicsApiSource}
			  Include/ThirdParty/imgui/backends/imgui_impl_dx12.cpp
			  )
		endif()

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



if (USE_IMGUI)
	target_compile_definitions(EugeneLib PUBLIC USE_IMGUI)
	target_include_directories(EugeneLib PUBLIC Include/ThirdParty/imgui)
endif()



 target_compile_definitions(EugeneLib PUBLIC NOMINMAX)
 
 # サンプルをビルドするか？
 if (BUILD_SAMPLE)
 add_subdirectory("Sample")
 endif()


 # Effekseerを使用するか？
 if (USE_EFFEKSEER)

	# 使用時
	add_subdirectory("Effekseer")
	target_link_libraries(EugeneLib Effekseer)
	target_link_libraries(EugeneLib LLGI)
	target_compile_definitions(EugeneLib PUBLIC USE_EFFEKSEER)
	target_include_directories(EugeneLib PUBLIC "Effekseer/Dev/Cpp/Effekseer")
	
	# Vulkan使用するか？
	if (USE_VULKAN)
		# VULKAN使用時
		target_link_libraries(EugeneLib EffekseerRendererVULKAN)
	else()
		# 
		target_link_libraries(EugeneLib EffekseerRendererDX12)
		target_include_directories(EugeneLib PUBLIC "Effekseer/Dev/Cpp/EffekseerRendererDX12")
	endif()

  else()

 endif()

add_subdirectory("Test")

