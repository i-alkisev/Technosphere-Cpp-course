project(httpLib)

set(SOURCE_FILES src/HttpService.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} pthread descriptor netLib)