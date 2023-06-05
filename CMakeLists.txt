cmake_minimum_required(VERSION 3.25)
project(multithreaded-server)

find_package(CURL REQUIRED)
include_directories(include)

file(GLOB SOURCES src/*.cpp)
configure_file(".env" "${CMAKE_BINARY_DIR}/.env" COPYONLY)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl ws2_32)