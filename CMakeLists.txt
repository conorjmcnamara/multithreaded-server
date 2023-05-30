cmake_minimum_required(VERSION 3.26.4)
project(multithreaded-server)

set(SOURCES 
    src/main.cpp 
    src/server.cpp 
    src/client.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ws2_32)