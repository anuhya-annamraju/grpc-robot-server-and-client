#include "TelemetryService.hpp"
#include <chrono>
#include <iostream>
#include <thread>

using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

Status TelemetryService::GetRobotStatus(
    ServerContext* context,
    const RobotStatusRequest* request,
    ServerWriter<RobotStatus>* writer
) {
    if (request->send_status()) {
        std::cout << "Client has requested to stream telemetry data" << std::endl;

        // Simulate an ongoing telemetry streaming update frame loop (e.g., sending 10 updates)
        for (int i = 0; i < 10; ++i) {
            if (context->IsCancelled()) {
                std::cout << "Client disconnected. Aborting telemetry stream loop." << std::endl;
                return Status::CANCELLED;
            }

            RobotStatus status;
            status.set_robotserialid(101);
            status.set_latitude(48.1351f + (i * 0.0005f));
            status.set_ismoving(true);

            writer->Write(status);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        std::cout << "Finished data push execution pipeline successfully." << std::endl;
    } else {
        std::cout << "Client has requested to stop streaming telemetry data" << std::endl;
    }

    return Status::OK;
}
