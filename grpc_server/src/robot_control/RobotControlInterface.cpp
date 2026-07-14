#include "RobotControlInterface.hpp"


RobotControlInterface::RobotControlInterface(asio::io_context& io_context) {
    ParseRobotSpecs();
    _parseTimer = new asio::steady_timer(io_context, _parseInterval);

}

RobotControlInterface::~RobotControlInterface() {
    _parseTimer->cancel();
}

void RobotControlInterface::StartParsingState() {

    _parseTimer->async_wait([this](const asio::error_code& error) {
        if (!error) {
            ParseRobotState();
            _parseTimer->expires_after(_parseInterval);
            StartParsingState();
        }
    });
}

void RobotControlInterface::ParseRobotSpecs() {
    // Parse the robot specifications from the serial data received from the robot
    // Update the robotSpecs struct with the parsed data
    _robotSpecs.robotId = "102";
    _robotSpecs.lifespan_hours = 10.0;
}

void RobotControlInterface::ParseRobotState() {
    // Parse the robot state from the serial data received from the robot
    // Update the robotState struct with the parsed data at 100 Hz freq
    switch (_moveCommand.direction) {
        case FORWARD:
            _robotState.pos[0] += _moveCommand.speed;
            break;
        case BACK:
            _robotState.pos[0] -= _moveCommand.speed;
            break;
        case LEFT:
            _robotState.pos[1] -= _moveCommand.speed;
            break;
        case RIGHT:
            _robotState.pos[1] += _moveCommand.speed;
            break;
        default:
            break;
    }

    _robotState.batteryLevel = std::max(0.0f, _robotState.batteryLevel - 0.01f); // Simulate battery drain
}



