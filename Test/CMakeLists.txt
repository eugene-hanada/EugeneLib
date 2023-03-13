cmake_minimum_required (VERSION 3.20)
project(Test)

# Catch2のディレクトリ追加
add_subdirectory(./Catch2)

include_directories(../Include)
link_directories(../Release)

# ソースをbuild
add_executable(tests test.cpp)

# Catch2が用意したmainにリンク
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

