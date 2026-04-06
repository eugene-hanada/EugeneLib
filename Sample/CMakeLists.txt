cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:WinMainCRTStartup")
	add_definitions(-DUNICODE -D_UNICODE -DWIN32 -D_WINDOWS)
endif()

project ("Sample")

# 実行ファイルを作成
add_executable (Sample WIN32  "main.cpp")

target_include_directories(Sample PRIVATE "../Include" ${EUGENE_EFFEKSEER_DIR}/include)
target_link_libraries(Sample EugeneLib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Sample PROPERTY CXX_STANDARD 23)
endif()

# HLSLシェーダをコンパイルしてexe出力ディレクトリにコピー
find_program(DXC_EXECUTABLE dxc REQUIRED)


set(HLSL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SHADER_OUTPUT_DIR "$<TARGET_FILE_DIR:Sample>")


if (EUGENE_USE_DX12)
# 頂点シェーダ
add_custom_command(TARGET Sample POST_BUILD
	COMMAND ${DXC_EXECUTABLE} -T vs_6_0 -E main "${HLSL_SOURCE_DIR}/VertexShader.hlsl" -Fo "${SHADER_OUTPUT_DIR}/VertexShader.vso"
	COMMENT "Compiling VertexShader.hlsl with dxc"
)

# ピクセルシェーダ
add_custom_command(TARGET Sample POST_BUILD
	COMMAND ${DXC_EXECUTABLE} -T ps_6_0 -E main "${HLSL_SOURCE_DIR}/PixelShader.hlsl" -Fo "${SHADER_OUTPUT_DIR}/PixelShader.pso"
	COMMENT "Compiling PixelShader.hlsl with dxc"
)
elseif(EUGENE_USE_VULKAN)

	# 頂点シェーダ
	add_custom_command(TARGET Sample POST_BUILD
		COMMAND glslc "${HLSL_SOURCE_DIR}/VertexShader.vert" -o "${SHADER_OUTPUT_DIR}/VertexShader.vso"
		COMMENT "Compiling VertexShader.glsl with glslc"
	)
	
	# ピクセルシェーダ
	add_custom_command(TARGET Sample POST_BUILD
		COMMAND glslc "${HLSL_SOURCE_DIR}/FragmentShader.frag" -o "${SHADER_OUTPUT_DIR}/PixelShader.pso"
		COMMENT "Compiling PixelShader.glsl with glslc"
	)
endif()

# Logo.pngをコピー
add_custom_command(TARGET Sample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${HLSL_SOURCE_DIR}/Logo.png" "${SHADER_OUTPUT_DIR}/Logo.png"
	COMMENT "Copying Logo.png"
)


