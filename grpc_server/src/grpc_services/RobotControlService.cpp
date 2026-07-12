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


        if(command.direction() == MoveCommand::Direction::MoveCommand_Direction_FORWARD)
        {
            std::cout << "Moving Forward" << std::endl;
            pos[0] -= 0.1; // Move position in x-direction   
        }
        else if(command.direction() == MoveCommand::Direction::MoveCommand_Direction_LEFT)
        {
            std::cout << "Moving Left" << std::endl;
            pos[1] += 0.1; // Move position in y-direction
        }
        else if(command.direction() == MoveCommand::Direction::MoveCommand_Direction_RIGHT)
        {
            std::cout << "Moving Right" << std::endl;
            pos[0] += 0.1; // Move position in x-direction
        }
        else if (command.direction() == MoveCommand::Direction::MoveCommand_Direction_BACK)
        {
            std::cout << "Moving Back" << std::endl;
            pos[1] -= 0.1; // Move position in y-direction
        }
        else
        {
            std::cout << "Unknown Direction" << std::endl;
        }
        response.set_pos_x(pos[0]);
        response.set_pos_y(pos[1]);
        response.set_pos_z(pos[2]);

        response.set_ang_vel_x(angVel[0]);
        response.set_ang_vel_y(angVel[1]);
        response.set_ang_vel_z(angVel[2]);

        stream->Write(response);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 100 Hz update rate 

    }
    return Status::OK;
}