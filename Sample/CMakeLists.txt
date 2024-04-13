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
