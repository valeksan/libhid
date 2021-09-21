#ifndef LIBHID_H
#define LIBHID_H

#include <string>

class Libhid {
 public:
    Libhid();

    static std::string GetHardwareId();
};

#endif // LIBHID_H
