#include "libhid/libhid_c.h"

#include <cstring>

int main()
{
    char hardwareId[64] = {0};
    char namespacedHardwareId[64] = {0};
    char shortBuffer[8] = {0};

    if (libhid_hardware_id_buffer_size() != 37u) {
        return 1;
    }

    if (libhid_get_hardware_id(nullptr, sizeof(hardwareId)) != LIBHID_STATUS_INVALID_ARGUMENT) {
        return 1;
    }

    if (libhid_get_hardware_id(shortBuffer, sizeof(shortBuffer)) != LIBHID_STATUS_BUFFER_TOO_SMALL
            || shortBuffer[0] != '\0') {
        return 1;
    }

    const libhid_status hardwareIdStatus = libhid_get_hardware_id(hardwareId, sizeof(hardwareId));
    if (hardwareIdStatus != LIBHID_STATUS_OK && hardwareIdStatus != LIBHID_STATUS_UNAVAILABLE) {
        return 1;
    }

    if (hardwareIdStatus == LIBHID_STATUS_OK && std::strlen(hardwareId) != 36u) {
        return 1;
    }

    const libhid_status namespacedStatus =
            libhid_get_hardware_id_with_namespace("libhid-c-api-smoke-test",
                                                  namespacedHardwareId,
                                                  sizeof(namespacedHardwareId));
    if (namespacedStatus != LIBHID_STATUS_OK && namespacedStatus != LIBHID_STATUS_UNAVAILABLE) {
        return 1;
    }

    if (hardwareIdStatus == LIBHID_STATUS_UNAVAILABLE && namespacedStatus != LIBHID_STATUS_UNAVAILABLE) {
        return 1;
    }

    if (hardwareIdStatus == LIBHID_STATUS_OK && namespacedStatus != LIBHID_STATUS_OK) {
        return 1;
    }

    if (hardwareIdStatus == LIBHID_STATUS_OK && std::strcmp(hardwareId, namespacedHardwareId) == 0) {
        return 1;
    }

    return 0;
}
