#include "RobotControlService.hpp"
#include <chrono>
#include <iostream>
#include <thread>

using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

Status RobotControlService::GetRobotId(
    ServerContext* context,
    const RobotIdRequest* request,
    RobotIdResponse* response
) {
    response->set_id("101");
    return Status::OK;
}


Status RobotControlService::GetBatteryLevel(
    ServerContext* context,
    const BatteryLevelRequest* request,
    ServerWriter<BatteryLevelResponse>* writer
) {
    for (int i = 0; i < 5; ++i) {
        BatteryLevelResponse response;
        response.set_percentage(100 - i * 10); // Simulate battery drain
        writer->Write(response);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate delay
    }
    return Status::OK;
}

Status RobotControlService::MoveRobotCommand(
    ServerContext* context,
    grpc::ServerReaderWriter<TelemetryResponse, MoveCommand>* stream
) {
    MoveCommand command;
    while (stream->Read(&command)) {
        TelemetryResponse response;

        response.set_latitude(37.7749);
        response.set_longitude(-122.4194);
        response.set_altitude(450.0);
        response.set_ang_vel_x(0.1);
        response.set_ang_vel_y(0.2);
        response.set_ang_vel_z(0.3);

        stream->Write(response);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 100 Hz update rate 

    }
    return Status::OK;
}