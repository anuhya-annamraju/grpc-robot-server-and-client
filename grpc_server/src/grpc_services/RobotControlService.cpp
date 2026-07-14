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
    BatteryLevelResponse response;
    response.set_percentage(_robotController->GetBatteryLevel());
    writer->Write(response);
    return Status::OK;
}

Status RobotControlService::MoveRobotCommand(
    ServerContext* context,
    grpc::ServerReaderWriter<TelemetryResponse, MoveCommand>* stream
) {
    MoveCommand command;
    while (stream->Read(&command)) {
        RobotControlInterface::MoveRobotCommand moveCommand;
        switch (command.direction()) {
            case MoveCommand::FORWARD:
                moveCommand.direction = RobotControlInterface::FORWARD;
                break;
            case MoveCommand::BACK:
                moveCommand.direction = RobotControlInterface::BACK;
                break;
            case MoveCommand::LEFT:
                moveCommand.direction = RobotControlInterface::LEFT;
                break;
            case MoveCommand::RIGHT:
                moveCommand.direction = RobotControlInterface::RIGHT;
                break;
                default:
                moveCommand.direction = RobotControlInterface::FORWARD;
                break;
        }
        moveCommand.speed = 1.0;
        _robotController->SetMoveCommand(moveCommand);
    return Status::OK;
}

