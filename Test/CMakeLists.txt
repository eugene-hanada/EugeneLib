cmake_minimum_required (VERSION 3.20)
project(tests)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

# ランタイムライブラリのリンクの仕方を設定
if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


include_directories(../Include)
add_subdirectory(Catch2)

# ソースをbuild
add_executable(tests test.cpp)

target_include_directories(tests PRIVATE "Catch2/src")
target_link_libraries(tests PRIVATE EugeneLib)
target_link_libraries(tests PUBLIC Catch2::Catch2)




