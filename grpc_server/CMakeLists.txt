cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(grpcserver VERSION 1.0)

add_executable(grpcserver src/main.cpp
    src/grpc_services/RobotControlService.cpp
    src/robot_control/RobotController.cpp
)

find_package(protobuf REQUIRED CONFIG)
find_package(gRPC REQUIRED CONFIG)

set(GRPC_CPP_PLUGIN_EXE "/opt/homebrew/bin/grpc_cpp_plugin")

protobuf_generate(
    TARGET grpcserver
    LANGUAGE cpp
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../proto_definitions   
    PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/../proto_definitions/robot_control_api.proto
)

protobuf_generate(
    TARGET grpcserver
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    # FIXED: Changed from gRPC_CPP_PLUGIN_EXECUTABLE to GRPC_CPP_PLUGIN_EXE
    PLUGIN "protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXE}"
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../proto_definitions   
    PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/../proto_definitions/robot_control_api.proto
)

target_link_libraries(grpcserver PRIVATE protobuf::libprotobuf gRPC::grpc++)
target_include_directories(grpcserver PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/src
# Modern approach: Extracts paths directly from the target links
    $<TARGET_PROPERTY:protobuf::libprotobuf,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:gRPC::grpc++,INTERFACE_INCLUDE_DIRECTORIES>
)