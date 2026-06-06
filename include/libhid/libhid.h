#ifndef LIBHID_H
#define LIBHID_H

#include "libhid/libhid_export.h"

#include <string>

#if !defined(SHAREDTESTLIB_EXPORT)
#  define SHAREDTESTLIB_EXPORT LIBHID_EXPORT
#endif

namespace libhid {

[[nodiscard]] LIBHID_EXPORT std::string GetHardwareId();
[[nodiscard]] LIBHID_EXPORT std::string GetHardwareId(const std::string &applicationNamespace);

} // namespace libhid

class LIBHID_EXPORT LibHid
{
public:
    LibHid();
    [[nodiscard]] static std::string GetHardwareId();
    [[nodiscard]] static std::string GetHardwareId(const std::string &applicationNamespace);
};



#endif // LIBHID_H
