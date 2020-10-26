project(LoggerLib)

set(SOURCE_FILES "src/LoggerLib.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)
