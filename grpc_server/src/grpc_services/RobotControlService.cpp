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
    response.set_percentage(_robotController.GetBatteryLevel());
    writer->Write(response);
    return Status::OK;
}

Status RobotControlService::MoveRobotCommand(
    ServerContext* context,
    grpc::ServerReaderWriter<TelemetryResponse, MoveCommand>* stream
) {
    MoveCommand command;
    switch (command.direction()) {
        case MoveCommand::FORWARD:
            _robotController.MoveRobotCommand(RobotController::MoveRobotCommand::FORWARD);
            break;
        case MoveCommand::BACK:
            _robotController.MoveRobotCommand(RobotController::MoveRobotCommand::BACK);
            break;
        case MoveCommand::LEFT:
            _robotController.MoveRobotCommand(RobotController::MoveRobotCommand::LEFT);
            break;
        case MoveCommand::RIGHT:
            _robotController.MoveRobotCommand(RobotController::MoveRobotCommand::RIGHT);
            break;
        default:
        break;
    }
    return Status::OK;
}

