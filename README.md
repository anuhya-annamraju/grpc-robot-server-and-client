# gRPC Server-Client prototype
![alt text](<Screenshot 2026-07-12 at 17.51.33.png>)
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

This repo was developed and tested on MAC, so it is recommended to use that for ease of use. 

## Prerequisites (All Platforms)
- CMake 3.10+
- C++17 compatible compiler
- gRPC and Protobuf libraries installed

### macOS
```bash
brew install cmake grpc protobuf asio
```

### Linux / WSL
```bash
sudo apt-get install cmake libgrpc-dev libprotobuf-dev protobuf-compiler libgrpc++1 grpc-tools asio-dev
```

### Windows (MSVC)
Install using vcpkg:
```bash
vcpkg install grpc:x64-windows protobuf:x64-windows asio:x64-windows
```

## Building the Server

### Method 1: VS Code Tasks (Recommended)
1. Install CMake Tools extension in VS Code
2. Run Task → Run Task → "2. Build C++ Server"

### Method 2: Command Line (Cross-Platform)
```bash
cd grpc_server
mkdir -p build
cd build
cmake ..
cmake --build . --config Release -j4
./grpcserver  # Linux/macOS
grpcserver.exe  # Windows
```

## Production Readiness & Design Decisions

### Current Implementation (MVP)
✅ **What's working:** Basic gRPC streaming, bi-directional communication, graceful shutdown, clean code practices  
✅ **Scope:** Proof-of-concept for robot telemetry and control  

### Design Trade-offs Made
- **Insecure Channel (Development):** Uses unencrypted gRPC for simplicity. Production requires TLS.
- **In-Memory State:** Robot state held in memory. Production needs persistent storage/database.
- **Single Robot:** Hard-coded robot ID. Production needs multi-robot support.
- **No Authentication:** No access control. Production needs OAuth/JWT tokens.
- **Synchronous RPC Handling:** Current model blocks on stream reads. Production may need async patterns for scale.

### Future Work / Production TODOs


**Testing & Quality**
- [ ] Unit tests (C++ and Python)
- [ ] Integration tests with mock robots
- [ ] CI/CD pipeline (GitHub Actions)
- [ ] Code coverage reporting


## How to launch the server (Legacy)
- The cpp based server was tested on mac, and can be build either manually from commandline or using the tasks.json

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



 


