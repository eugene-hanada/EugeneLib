cmake_minimum_required (VERSION 3.20)
project(tests)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

# ランタイムライブラリのリンクの仕方を設定
if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(EUGENE_USE_SYSTEM_TEST "システムのテストをビルドするか?" OFF)
option(EUGENE_USE_GRAPHICS_TEST "グラフィックスのテストをビルドするか？" OFF)




add_executable(tests ./test.cpp)
target_include_directories(tests PRIVATE "./doctest")
target_include_directories(tests PRIVATE "../Include")
target_link_libraries(tests PRIVATE EugeneLib)
target_compile_definitions(EugeneLib PUBLIC EUGENE_TEST)

if (EUGENE_USE_SYSTEM_TEST)
	target_compile_definitions(tests PRIVATE EUGENE_USE_SYSTEM_TEST)
endif()
if (EUGENE_USE_GRAPHICS_TEST)
	target_compile_definitions(tests PRIVATE EUGENE_USE_GRAPHICS_TEST)
endif()
