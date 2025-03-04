cmake_minimum_required(VERSION 3.25)

project(TestProject)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "${CMAKE_SOUCE_DIR}/include/SDL3")

add_executable(TestProject ./src/main.cpp ./src/renderer.cpp)
target_link_libraries(TestProject PRIVATE ${CMAKE_SOURCE_DIR}/include/SDL3/libSDL3.so)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/SDL3)
link_directories(${CMAKE_SOURCE_DIR}/include/SDL3)

include_directories(/usr/local/include)