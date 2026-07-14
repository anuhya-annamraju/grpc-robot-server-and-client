#pragma once
#include <grpcpp/grpcpp.h>
#include "robot_control_api.grpc.pb.h"
#include "robot_control/RobotControlInterface.hpp"

class RobotControlService final : public RobotControlServices::Service {
public:
    RobotControlService(std::unique_ptr<RobotControlInterface> controller) : _robotController(std::move(controller)) {};
    ~RobotControlService();
    grpc::Status GetRobotId(grpc::ServerContext* context, const RobotIdRequest* request, RobotIdResponse* response) override ;
    grpc::Status GetBatteryLevel(grpc::ServerContext* context, const BatteryLevelRequest* request, grpc::ServerWriter<BatteryLevelResponse>* writer) override ;
    grpc::Status MoveRobotCommand(grpc::ServerContext* context, grpc::ServerReaderWriter<TelemetryResponse, MoveCommand>* stream) override ;
private:
std::unique_ptr<RobotControlInterface> _robotController;
TelemetryResponse _telemetryResponse;

void ParseTelemetryData();

};

