cmake_minimum_required(VERSION 3.25)

project(TestProject)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "/path/to/your/sdl3")

add_executable(TestProject ./src/main.cpp ./src/renderer.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
target_link_libraries(TestProject SDL3)