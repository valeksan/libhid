#include "libhid/libhid.h"

#include <iostream>
#include <string>

int main()
{
    const std::string hardwareId = LibHid::GetHardwareId();

    if (hardwareId.empty()) {
        std::cout << "Hardware ID is not available" << std::endl;
        return 0;
    }

    std::cout << hardwareId << std::endl;
    return 0;
}
