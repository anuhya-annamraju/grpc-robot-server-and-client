cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform detection for cross-compilation support
if(WIN32)
    message(STATUS "Building for Windows")
    set(PLATFORM_SPECIFIC_LIBS ws2_32)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux/WSL")
    set(PLATFORM_SPECIFIC_LIBS pthread)
elseif(APPLE)
    message(STATUS "Building for macOS")
    set(PLATFORM_SPECIFIC_LIBS "")
endif()

project(grpcserver VERSION 1.0)

add_executable(grpcserver src/main.cpp
    src/grpc_services/RobotControlService.cpp
    src/robot_control/RobotControlInterface.cpp
)

find_package(protobuf REQUIRED CONFIG)
find_package(gRPC REQUIRED CONFIG)

# Robust gRPC plugin detection (works across macOS/Linux/Windows)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin REQUIRED)
message(STATUS "gRPC C++ plugin: ${GRPC_CPP_PLUGIN_EXECUTABLE}")

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
    PLUGIN "protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}"
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../proto_definitions   
    PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/../proto_definitions/robot_control_api.proto
)

target_link_libraries(grpcserver PRIVATE protobuf::libprotobuf gRPC::grpc++ ${PLATFORM_SPECIFIC_LIBS})
target_include_directories(grpcserver PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/src
# Modern approach: Extracts paths directly from the target links
    $<TARGET_PROPERTY:protobuf::libprotobuf,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:gRPC::grpc++,INTERFACE_INCLUDE_DIRECTORIES>
)