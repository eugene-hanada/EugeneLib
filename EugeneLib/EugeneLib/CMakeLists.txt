cmake_minimum_required (VERSION 3.20)
project(EugeneLib)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

# visual stdio用にunicodeを使うように
add_definitions(-D_UNICODE -DUNICODE)

# システム系のクラスのソースファイルをセット
set(SystemSource 
    Source/System/System.cpp
    Source/System/Windows/WindowsSystem.cpp
    Source/System/Windows/Dll.cpp
    Source/System/GamePad.cpp
    Source/System/DynamicLibrary.cpp
)

# グラフィック系クラスのソースファイルをセット
set(GraphicsSource
    Source/Graphics/Graphics.cpp
    Source/Graphics/GpuEngine.cpp
    Source/Graphics/CommandList.cpp
    Source/Graphics/GpuResource.cpp
    Source/Graphics/GraphicsPipeline.cpp
    Source/Graphics/RenderTargetViews.cpp
    Source/Graphics/DepthStencilViews.cpp
    Source/Graphics/ShaderResourceViews.cpp
    Source/Graphics/VertexView.cpp
    Source/Graphics/IndexView.cpp
    Source/Graphics/Shader.cpp
    Source/Graphics/Texture.cpp
)

# グラフィック系の実装クラスのソースファイルをセット
set(GraphicsApiSource
    Source/Graphics/DirectX12/Dx12CommandList.cpp
    Source/Graphics/DirectX12/Dx12DefaultResource.cpp
    Source/Graphics/DirectX12/Dx12DepthStencilViews.cpp
    Source/Graphics/DirectX12/Dx12GpuEngine.cpp
    Source/Graphics/DirectX12/Dx12GpuResource.cpp
    Source/Graphics/DirectX12/Dx12Graphics.cpp
    Source/Graphics/DirectX12/Dx12GraphicsPipeline.cpp
    Source/Graphics/DirectX12/Dx12RenderTargetViews.cpp
    Source/Graphics/DirectX12/Dx12ShaderResourceViews.cpp
    Source/Graphics/DirectX12/Dx12UploadableResource.cpp
    Source/Graphics/DirectX12/Dx12VertexView.cpp
    Source/Graphics/DirectX12/Dx12IndexView.cpp
    Source/Graphics/DirectX12/Dx12Views.cpp
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
 )

# サウンド系のクラスのソースファイルをセット
set(SoundSource
    Source/Sound/Sound.cpp
    Source/Sound/SoundBase.cpp
    Source/Sound/SoundSpeaker.cpp
    Source/Sound/SoundControl.cpp
     Source/Sound/Sound3DControl.cpp
    Source/Sound/Wave.cpp
)

# サウンド系の実装クラスのソースファイルをセット
set(SoundApiSource
    Source/Sound/Xaudio2/Xa2Sound.cpp
    Source/Sound/Xaudio2/Xa2SoundSpeaker.cpp
    Source/Sound/Xaudio2/Xa2SoundControl.cpp
    Source/Sound/Xaudio2/Xa2Sound3DControl.cpp
)

# DirectXTexをセット
set(DirectXTexSource
    DirectXTex/DirectXTex/DirectXTex.h
    DirectXTex/DirectXTex/DirectXTex.inl
    DirectXTex/DirectXTex/BC.h
    DirectXTex/DirectXTex/DDS.h
    DirectXTex/DirectXTex/DirectXTexP.h
    DirectXTex/DirectXTex/filters.h
    DirectXTex/DirectXTex/scoped.h
    DirectXTex/DirectXTex/BC.cpp
    DirectXTex/DirectXTex/BC4BC5.cpp
    DirectXTex/DirectXTex/BC6HBC7.cpp
    DirectXTex/DirectXTex/DirectXTexCompress.cpp
    DirectXTex/DirectXTex/DirectXTexConvert.cpp
    DirectXTex/DirectXTex/DirectXTexDDS.cpp
    DirectXTex/DirectXTex/DirectXTexHDR.cpp
    DirectXTex/DirectXTex/DirectXTexImage.cpp
    DirectXTex/DirectXTex/DirectXTexMipmaps.cpp
    DirectXTex/DirectXTex/DirectXTexMisc.cpp
    DirectXTex/DirectXTex/DirectXTexNormalMaps.cpp
    DirectXTex/DirectXTex/DirectXTexPMAlpha.cpp
    DirectXTex/DirectXTex/DirectXTexResize.cpp
    DirectXTex/DirectXTex/DirectXTexTGA.cpp
    DirectXTex/DirectXTex/DirectXTexUtil.cpp
    DirectXTex/DirectXTex/DirectXTexWIC.cpp
    DirectXTex/DirectXTex/d3dx12.h
    DirectXTex/DirectXTex/DirectXTexD3D12.cpp
)

add_library(EugeneLib STATIC 
    ${SystemSource}
    ${GraphicsSource}
    ${GraphicsApiSource}
    ${CommonSource}
    ${MathSource}
    ${SoundSource}
    ${SoundApiSource}
    ${DirectXTexSource}
)

# ランタイムライブラリを静的にリンクするように
set_property(TARGET EugeneLib PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
