#include <iostream>
#include <memory>
#include <string>
#include <csignal>
#include <thread>
#include "grpc_services/RobotControlService.hpp"


std::atomic<bool> keep_server_running(true);
void terminate_server(int signal_num)
{
    if(signal_num == SIGINT)
    {
        std::cout << "SIGINT received. Terminating server..." << std::endl;
        keep_server_running = false;
        exit(0);
    }
}

void RunServer() {
    // Listen on port 12345 on all local network interfaces
    std::string server_address("0.0.0.0:12345");
    
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

    while (keep_server_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}

int main() {

    std::signal(SIGINT, terminate_server); 
    RunServer();
    return 0;
}