#include "libhid/libhid.h"

#if defined(OS_WINDOWS)
#include "imp/windows_manager.hpp"
#elif defined(OS_LINUX)
#include "imp/linux_manager.hpp"
#elif defined(OS_MAC)
#include "imp/mac_manager.hpp"
#endif
#include "imp/hardware_id_builder.hpp"

#include <iostream>

using namespace system_info;

namespace {

#ifdef LIB_DEBUG
void LogHardwareProbeDiagnostics(const HardwareProbeResult &probeResult)
{
    std::cerr << "Hardware source diagnostics:" << std::endl;
    for (const HardwareProbeItem &item : probeResult.items) {
        std::cerr << "  - " << item.source << ": "
                  << (item.available ? "found" : "missing") << std::endl;
    }
}
#endif

} // namespace

LibHid::LibHid()
{
}

std::string libhid::GetHardwareId()
{
    return GetHardwareId(std::string());
}

std::string libhid::GetHardwareId(const std::string &applicationNamespace)
{
    std::string hardwareIdResult = "";
    try {
        // Getting HID
        const HardwareProbeResult probeResult = NativeOSManager::GetHardwareProbeResult();
        std::string hid = probeResult.combinedProperties;
        if (hid.empty()) {
#ifdef LIB_DEBUG
            std::cerr << "Couldn't get any of all hardware IDs! " << std::endl;
            LogHardwareProbeDiagnostics(probeResult);
#endif
            return hid;
        }
        hardwareIdResult = BuildHardwareIdFromProperties(hid, applicationNamespace);
    } catch (...) {
#ifdef LIB_DEBUG
        std::cerr << "Unpredictable error in GetHardwareId! " << std::endl;
#endif
    }
    return hardwareIdResult;
}

std::string LibHid::GetHardwareId()
{
    return libhid::GetHardwareId();
}

std::string LibHid::GetHardwareId(const std::string &applicationNamespace)
{
    return libhid::GetHardwareId(applicationNamespace);
}
