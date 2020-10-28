cmake_minimum_required(VERSION 3.10)

project(task3)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-Wall)

add_executable(${PROJECT_NAME} src/main.cpp)

add_subdirectory(libs)

target_link_libraries(${PROJECT_NAME} TCPLib)