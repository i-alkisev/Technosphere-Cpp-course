cmake_minimum_required(VERSION 3.10)

project(process)

set(SOURCE_FILES "src/process.cpp")
set(HEADER_FILES "include/process.hpp")

include_directories("include/")

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})