#pragma once
#include <string>



class RobotController {

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

    enum MoveRobotCommand
    {
            FORWARD,
            BACK,
            LEFT,
            RIGHT
    };
    RobotController();
    ~RobotController();

    std::string GetRobotId(){return _robotSpecs.robotId;};
    float GetBatteryLevel(){return _robotState.batteryLevel;};
    void MoveRobotCommand(const MoveRobotCommand& command);

    private:

    void ParseRobotState();
    void ParseRobotSpecs();
    void ParseBatteryLevel();
    RobotSpecs _robotSpecs;
    RobotState _robotState;

};