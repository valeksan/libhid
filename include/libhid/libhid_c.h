#ifndef LIBHID_C_H
#define LIBHID_C_H

#include "libhid/libhid_export.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum libhid_status {
    LIBHID_STATUS_OK = 0,
    LIBHID_STATUS_UNAVAILABLE = 1,
    LIBHID_STATUS_INVALID_ARGUMENT = 2,
    LIBHID_STATUS_BUFFER_TOO_SMALL = 3
} libhid_status;

LIBHID_EXPORT size_t libhid_hardware_id_buffer_size(void);
LIBHID_EXPORT libhid_status libhid_get_hardware_id(char *buffer, size_t buffer_size);
LIBHID_EXPORT libhid_status libhid_get_hardware_id_with_namespace(const char *application_namespace,
                                                                  char *buffer,
                                                                  size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif // LIBHID_C_H
