cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(grpcserver VERSION 1.0)
add_executable(grpcserver main.cpp )


find_package(protobuf REQUIRED CONFIG)
find_package(gRPC REQUIRED CONFIG)

set(GRPC_CPP_PLUGIN_EXE "/opt/homebrew/bin/grpc_cpp_plugin")

protobuf_generate(
    TARGET grpcserver
    LANGUAGE cpp
    PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/definitions/robot_status.proto
)

protobuf_generate(
    TARGET grpcserver
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    # FIXED: Changed from gRPC_CPP_PLUGIN_EXECUTABLE to GRPC_CPP_PLUGIN_EXE
    PLUGIN "protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXE}"
    PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/definitions/robot_status.proto
)

target_link_libraries(grpcserver  PRIVATE protobuf::libprotobuf gRPC::grpc++)
target_include_directories(grpcserver PRIVATE ${CMAKE_CURRENT_BINARY_DIR})