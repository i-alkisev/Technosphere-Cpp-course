cmake_minimum_required(VERSION 3.10)

project(task1)

set(CMAKE_CXX_STANDART 17)
add_definitions(-Wall)

include_directories("libs/process/include/")

add_executable(${PROJECT_NAME} src/main.cpp)

add_subdirectory(libs)

target_link_libraries(${PROJECT_NAME} process)
