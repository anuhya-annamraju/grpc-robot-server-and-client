#pragma once
#include <string>
#include <asio.hpp>

class RobotControlInterface {

    public :
    struct RobotSpecs
    {
        std::string robotId;
        float lifespan_hours = 10.0;
    };

    struct RobotState
    {
        float batteryLevel = 100.0f;
        float pos[3];
        float angVel[3];
    };

    enum Direction
    {
            FORWARD,
            BACK,
            LEFT,
            RIGHT
    };

    struct MoveRobotCommand
    {
        Direction direction;
        float speed;
    };

    RobotControlInterface(asio::io_context& io_context);
    ~RobotControlInterface();

    std::string GetRobotId(){return _robotSpecs.robotId;};
    float GetBatteryLevel(){return _robotState.batteryLevel;};
    void SetMoveCommand(const MoveRobotCommand& command){_moveCommand = command;};
    RobotState GetTelemetryData(){ParseRobotState(); return _robotState;};

    private:

    void ParseRobotState();
    void ParseRobotSpecs();
    void ParseBatteryLevel();
    void StartParsingState();
    RobotSpecs _robotSpecs;
    RobotState _robotState;
    MoveRobotCommand _moveCommand;

    asio::steady_timer* _parseTimer;
    std::chrono::milliseconds _parseInterval{10}; // 100 Hz freq

};