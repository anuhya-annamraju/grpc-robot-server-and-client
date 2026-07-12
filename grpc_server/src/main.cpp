#include <iostream>
#include <memory>
#include <string>

// Include your custom service header
#include "grpc_services/RobotControlService.hpp"

void RunServer() {
    // Listen on port 50051 on all local network interfaces
    std::string server_address("0.0.0.0:50051");
    
    // Instantiate your custom service class
    RobotControlService service;

    grpc::ServerBuilder builder;
    
    // Set up the socket address with insecure credentials (no SSL for local testing)
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
    // Register your service instance
    builder.RegisterService(&service);

    // Assemble and boot up the server process
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "gRPC Robot Server listening on " << server_address << std::endl;

    // Keep the application running and listening indefinitely
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}