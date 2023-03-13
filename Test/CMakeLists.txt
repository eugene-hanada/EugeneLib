cmake_minimum_required (VERSION 3.20)
project(tests)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

# ランタイムライブラリのリンクの仕方を設定
if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Catch2のディレクトリ追加
add_subdirectory(./Catch2)

include_directories(../Include)
link_directories(../Release)

# ソースをbuild
add_executable(tests test.cpp)

# Catch2が用意したmainにリンク
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)


