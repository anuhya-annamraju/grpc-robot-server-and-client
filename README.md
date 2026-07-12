# grpc server client prototype
This mono repo can launch a c++ based gRPC server app and a python based gRPC client app.

The server represents a cpp app that handles sensor data of the robot.
The client represents a GUI that end user can use to send commands to the server to control the robot and also visualize the state of the robot.

# API implemented in the server
Proto messages defined in this API are

- RobotIdRequest
- RobotIdResponse
- BatteryLevelRequest
- BatteryLevelResponse
- MoveCommand
- TelemetryResponse

The API offers 3 basic services for a robotoc application. 

1. A Request-Response type service - Python Client requests the robot Id and Server sends back a string

2. A Unidirectional server streaming service - Python client requests once the battery level and server sends the battery level every minute

3. A bidirectional server-client streaming service - Python client can send commands to move the robot (Forward, Left, Right & Back) and server sends the state of telemetry data as a response. 

# How to launch the server 
- The cpp based server uses cmake, so the mono repo can be opened in VS code and cpp app can be build either manually from commandline or using the tasks.json

1. Install cmake tools in VS code.
2. To build and run use tasks.json, 
    - press Ctrl*Shift+P or Cmd+Shift+P and then select Run>Tasks from the drop down and then select "2. Build C++ Server" option. 
    - To launch it, press Ctrl*Shift+P or Cmd+Shift+P and then select Run>Tasks from the drop down and then select "3. Start C++ Server" option.

3. To run manually from comamndline, perform
    - cd grpc_server 
    - mkdir build 
    - cd build 
    - cmake ..
    - cmake --build .
    To run it,
    - ./grpcserver


# How to launch python client
1. To build and run use tasks.json, 
    - press Ctrl*Shift+P or Cmd+Shift+P and then select Run>Tasks from the drop down and then select "1. Generate Python Protos" option. 
    - To launch it, press Ctrl*Shift+P or Cmd+Shift+P and then select Run>Tasks from the drop down and then select "4. Launch Python Dashboard" option.
 


