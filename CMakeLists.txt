cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(grpcserver VERSION 1.0)
add_executable(grpcserver main.cpp )

find_package(protobuf REQUIRED CONFIG)
protobuf_generate(
    TARGET grpcserver
    LANGUAGE cpp
    PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/definitions/robot_status.proto
)
target_link_libraries(grpcserver  PRIVATE protobuf::libprotobuf)
target_include_directories(grpcserver PRIVATE ${CMAKE_CURRENT_BINARY_DIR})