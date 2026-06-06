#include "libhid/libhid.h"
#include "libhid/libhid_c.h"

#if defined(OS_WINDOWS)
#include "imp/windows_manager.hpp"
#elif defined(OS_LINUX)
#include "imp/linux_manager.hpp"
#elif defined(OS_MAC)
#include "imp/mac_manager.hpp"
#endif
#include "imp/hardware_id_builder.hpp"

#include <cstring>
#include <iostream>

using namespace system_info;

namespace {

constexpr std::size_t kHardwareIdBufferSize = 37;

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

libhid_status CopyHardwareIdToBuffer(const std::string &applicationNamespace,
                                     char *buffer,
                                     std::size_t bufferSize)
{
    if (buffer == nullptr || bufferSize == 0) {
        return LIBHID_STATUS_INVALID_ARGUMENT;
    }

    const std::string hardwareId = libhid::GetHardwareId(applicationNamespace);
    if (hardwareId.empty()) {
        buffer[0] = '\0';
        return LIBHID_STATUS_UNAVAILABLE;
    }

    if (bufferSize < hardwareId.size() + 1) {
        buffer[0] = '\0';
        return LIBHID_STATUS_BUFFER_TOO_SMALL;
    }

    std::memcpy(buffer, hardwareId.c_str(), hardwareId.size() + 1);
    return LIBHID_STATUS_OK;
}

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

size_t libhid_hardware_id_buffer_size(void)
{
    return kHardwareIdBufferSize;
}

libhid_status libhid_get_hardware_id(char *buffer, size_t buffer_size)
{
    return CopyHardwareIdToBuffer(std::string(), buffer, buffer_size);
}

libhid_status libhid_get_hardware_id_with_namespace(const char *application_namespace,
                                                    char *buffer,
                                                    size_t buffer_size)
{
    return CopyHardwareIdToBuffer(application_namespace != nullptr ? std::string(application_namespace) : std::string(),
                                  buffer,
                                  buffer_size);
}
