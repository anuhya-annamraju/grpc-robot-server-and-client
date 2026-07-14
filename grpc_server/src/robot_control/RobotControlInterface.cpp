#include "RobotControlInterface.hpp"
#include <thread>

RobotControlInterface::RobotControlInterface() {
    ParseRobotSpecs();
}

RobotControlInterface::~RobotControlInterface() {
    
}
void RobotControlInterface::ParseRobotSpecs() {
    // Parse the robot specifications from the serial data received from the robot
    // Update the robotSpecs struct with the parsed data
    _robotSpecs.robotId = "102";
    _robotSpecs.lifespan_hours = 10.0;
}

void RobotControlInterface::ParseRobotState() {
    // Parse the robot state from the serial data received from the robot
    // Update the robotState struct with the parsed data
}

void RobotControlInterface::ParseBatteryLevel() {
    // Parse the battery level from the serial data received from the robot
    // Update the robotState struct with the parsed data
    // Simulate battery level
    for (int i = 0; i < 5; ++i) {
        _robotState.batteryLevel = 100 - i * 10; // Simulate battery drain
        std::this_thread::sleep_for(std::chrono::minutes(1)); // Simulate delay
    }
}


