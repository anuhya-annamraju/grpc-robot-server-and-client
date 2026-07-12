#pragma once
#include <grpcpp/grpcpp.h>
#include "robot_control_api.grpc.pb.h"

class RobotControlService final : public RobotControlServices::Service {
public:
    grpc::Status GetRobotId(grpc::ServerContext* context, const RobotIdRequest* request, RobotIdResponse* response) override ;
    grpc::Status GetBatteryLevel(grpc::ServerContext* context, const BatteryLevelRequest* request, grpc::ServerWriter<BatteryLevelResponse>* writer) override ;
    grpc::Status MoveRobotCommand(grpc::ServerContext* context, grpc::ServerReaderWriter<TelemetryResponse, MoveCommand>* stream) override ;
private:

float pos[3] = {0.0, 0.0, 0.25};
float angVel[3] = {0.0, 0.0, 0.0};

};

