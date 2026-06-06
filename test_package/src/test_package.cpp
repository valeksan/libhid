#include "libhid/libhid.h"

#include <string>

int main()
{
    const std::string hardwareId = libhid::GetHardwareId();
    const std::string namespacedHardwareId = libhid::GetHardwareId("conan-test-package");

    if (!hardwareId.empty() && namespacedHardwareId.empty()) {
        return 1;
    }

    if (!hardwareId.empty() && hardwareId == namespacedHardwareId) {
        return 1;
    }

    return 0;
}
