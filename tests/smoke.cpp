#include "libhid/libhid.h"

#include <string>

int main()
{
    const std::string hardwareId = libhid::GetHardwareId();
    const std::string appHardwareId = libhid::GetHardwareId("libhid-smoke-test");

    if (LibHid::GetHardwareId() != hardwareId) {
        return 1;
    }

    if (!hardwareId.empty() && appHardwareId.empty()) {
        return 1;
    }

    if (!hardwareId.empty() && hardwareId == appHardwareId) {
        return 1;
    }

    return 0;
}
