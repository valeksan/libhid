#include "libhid/libhid.h"

#include <string>

int main()
{
    const std::string hardwareId = LibHid::GetHardwareId();
    const std::string appHardwareId = LibHid::GetHardwareId("libhid-smoke-test");

    if (!hardwareId.empty() && appHardwareId.empty()) {
        return 1;
    }

    if (!hardwareId.empty() && hardwareId == appHardwareId) {
        return 1;
    }

    return 0;
}
