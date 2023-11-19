cmake_minimum_required (VERSION 3.20)
project(tests)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

# ランタイムライブラリのリンクの仕方を設定
if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()





add_executable(tests ./test.cpp)
target_include_directories(tests PRIVATE "./doctest")
target_include_directories(tests PRIVATE "../Include")
target_link_libraries(tests PRIVATE EugeneLib)

