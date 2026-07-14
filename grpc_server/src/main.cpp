#include <iostream>
#include <memory>
#include <string>
#include <csignal>
#include <thread>
#include <atomic>
#include <asio.hpp>
#include <grpcpp/grpcpp.h> // Ensure gRPC headers are visible

#include "grpc_services/RobotControlService.hpp"

// Global pointer to allow the signal handler to gracefully stop the server
std::unique_ptr<grpc::Server> global_server = nullptr;
std::shared_ptr<asio::io_context> global_io_context = nullptr;

void terminate_server(int signal_num)
{
    if (signal_num == SIGINT)
    {
        std::cout << "\nSIGINT received. Shutting down gracefully..." << std::endl;
        
        // 1. Stop the ASIO io_context so its background thread exits
        if (global_io_context) {
            global_io_context->stop();
        }

        // 2. Shut down the gRPC server cleanly
        if (global_server) {
            // Give active RPCs up to 2 seconds to wrap up before forcing shutdown
            global_server->Shutdown(std::chrono::system_clock::now() + std::chrono::seconds(2));
        }
    }
}

void RunServer() {
    std::string server_address("0.0.0.0:12345");
    
    // 1. Initialize ASIO
    global_io_context = std::make_shared<asio::io_context>();
    auto work_guard = asio::make_work_guard(*global_io_context);

    // 2. Spin up a background thread to process ASIO timers and events
    std::thread asio_thread([]() {
        global_io_context->run();
    });

    // 3. Construct your service dependencies
    auto robotController = std::make_unique<RobotControlInterface>(*global_io_context);
    // Passing the unique_ptr via std::move is correct here
    auto service = std::make_unique<RobotControlService>(std::move(robotController));

    // 4. Configure and Build the gRPC Server
    grpc::ServerBuilder builder; // Using stack allocation for builder is standard practice
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
    // CORRECTION: Pass raw pointer using '.get()' instead of the unique_ptr wrapper
    builder.RegisterService(service.get());

    global_server = builder.BuildAndStart();
    if (!global_server) {
        std::cerr << "Failed to start gRPC Server on " << server_address << std::endl;
        global_io_context->stop();
        if (asio_thread.joinable()) asio_thread.join();
        return;
    }
    
    std::cout << "gRPC Robot Server listening on " << server_address << std::endl;

    // 5. This blocks the main thread cleanly until global_server->Shutdown() is called in the signal handler
    global_server->Wait();

    // 6. Post-shutdown cleanup
    std::cout << "Cleaning up threads..." << std::endl;
    work_guard.reset(); // Release the work guard so the run loop can exit naturally
    global_io_context->stop();
    
    if (asio_thread.joinable()) {
        asio_thread.join();
    }
    std::cout << "Server shutdown complete." << std::endl;
}

int main() {
    // Register signal handler to gracefully interrupt the execution block
    std::signal(SIGINT, terminate_server); 
    
    RunServer();
    return 0;
}