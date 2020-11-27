project(netLib)

set(SOURCE_FILES src/BufferedConnection.cpp
                 src/Connection.cpp
                 src/EPoll.cpp
                 src/ErrnoExcept.cpp
                 src/Server.cpp
                 src/Service.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} descriptor)