#ifndef LIBHID_H
#define LIBHID_H

#include <string>

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define LIBHID_DECL_EXPORT __declspec(dllexport)
#  define LIBHID_DECL_IMPORT __declspec(dllimport)
#else
#  define LIBHID_DECL_EXPORT __attribute__((visibility("default")))
#  define LIBHID_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(SHAREDTESTLIB_LIBRARY) && !defined(LIBHID_LIBRARY)
#  define LIBHID_LIBRARY
#endif

#if defined(LIBHID_LIBRARY)
#  define LIBHID_EXPORT LIBHID_DECL_EXPORT
#else
#  define LIBHID_EXPORT LIBHID_DECL_IMPORT
#endif

#if !defined(SHAREDTESTLIB_EXPORT)
#  define SHAREDTESTLIB_EXPORT LIBHID_EXPORT
#endif

class LIBHID_EXPORT LibHid
{
public:
    LibHid();
    [[nodiscard]] static std::string GetHardwareId();
};



#endif // LIBHID_H
