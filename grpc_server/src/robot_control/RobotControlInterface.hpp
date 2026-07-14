#pragma once
#include <string>



class RobotControlInterface {

    public :
    struct RobotSpecs
    {
        std::string robotId;
        float lifespan_hours = 10.0;
    };

    struct RobotState
    {
        float batteryLevel;
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

    RobotControlInterface();
    ~RobotControlInterface();

    std::string GetRobotId(){return _robotSpecs.robotId;};
    float GetBatteryLevel(){return _robotState.batteryLevel;};
    void SetMoveCommand(const MoveRobotCommand& command){_moveCommand = command;};
    RobotState GetTelemetryData(){ParseRobotState(); return _robotState;};

    private:

    void ParseRobotState();
    void ParseRobotSpecs();
    void ParseBatteryLevel();
    RobotSpecs _robotSpecs;
    RobotState _robotState;
    MoveRobotCommand _moveCommand;

};