cmake_minimum_required (VERSION 3.20)
project(tests)
Include(FetchContent)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

# ランタイムライブラリのリンクの仕方を設定
if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()



FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v3.3.2 # or a later release
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests ./test.cpp)
target_link_directories(tests PRIVATE "../Debug")
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE "../Include")


