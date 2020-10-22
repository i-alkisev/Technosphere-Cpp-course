project(TCPLib)

file(GLOB SOURCE_FILES "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)