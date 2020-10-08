cmake_minimum_required(VERSION 3.10)

project(task1)

set(CMAKE_CXX_STANDART 17)
add_definitions(-Wall)

add_subdirectory(lib)

add_executable(${PROJECT_NAME} src/main.cpp)

include_directories("/lib/process/include")

target_link_libraries(${PROJECT_NAME} process)
