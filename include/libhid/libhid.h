#ifndef LIBHID_H
#define LIBHID_H

#include "libhid/libhid_export.h"

#include <string>

#if defined(__has_cpp_attribute)
#  if __has_cpp_attribute(nodiscard) && __cplusplus >= 201703L
#    define LIBHID_NODISCARD [[nodiscard]]
#  endif
#endif

#if !defined(LIBHID_NODISCARD)
#  if defined(__GNUC__) || defined(__clang__)
#    define LIBHID_NODISCARD __attribute__((warn_unused_result))
#  else
#    define LIBHID_NODISCARD
#  endif
#endif

#if !defined(SHAREDTESTLIB_EXPORT)
#  define SHAREDTESTLIB_EXPORT LIBHID_EXPORT
#endif

namespace libhid {

LIBHID_NODISCARD LIBHID_EXPORT std::string GetHardwareId();
LIBHID_NODISCARD LIBHID_EXPORT std::string GetHardwareId(const std::string &applicationNamespace);

} // namespace libhid

class LIBHID_EXPORT LibHid
{
public:
    LibHid();
    LIBHID_NODISCARD static std::string GetHardwareId();
    LIBHID_NODISCARD static std::string GetHardwareId(const std::string &applicationNamespace);
};



#endif // LIBHID_H
