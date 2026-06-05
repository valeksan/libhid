#include "libhid/libhid.h"

#include <string>

int main()
{
    const std::string hardwareId = LibHid::GetHardwareId();
    (void)hardwareId;
    return 0;
}
