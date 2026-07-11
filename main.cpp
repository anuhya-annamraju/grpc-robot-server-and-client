#include <stdio.h>
#include "definitions/robot_status.pb.h"

int main() {
    printf("Hello, World!\n");
    robot_status status;

    status.set_robotserialid(12345);
    status.set_latitude(37.7749);
    status.set_ismoving(true);

    std::cout << "robot id is " << status.robotserialid() << std::endl;
    return 0;
}