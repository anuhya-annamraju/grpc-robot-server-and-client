#include <stdio.h>
#include "definitions/robot_status.pb.h"

int main() {
    printf("Hello, World!\n");
    RobotStatus status;

    status.set_robotserialid(1234);
    status.set_latitude(37.7749);
    status.set_ismoving(true);

    std::string serialized;

   if( !status.SerializeToString(&serialized) ) {
        std::cerr << "Failed to serialize status." << std::endl;
        return 1;
    }

    std::cout << "robot id is " << status.robotserialid() << std::endl;
    return 0;
}