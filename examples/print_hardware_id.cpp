#include "libhid/libhid.h"

#include <iostream>
#include <string>

int main()
{
    const std::string hardwareId = libhid::GetHardwareId();
    const std::string appHardwareId = libhid::GetHardwareId("example-app");

    if (hardwareId.empty()) {
        std::cout << "Hardware ID is not available" << std::endl;
        return 0;
    }

    std::cout << "Hardware ID: " << hardwareId << std::endl;
    std::cout << "Application-specific hardware ID: " << appHardwareId << std::endl;
    return 0;
}
