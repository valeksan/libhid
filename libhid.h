#ifndef LIBHID_H
#define LIBHID_H

#include <QString>

class Libhid
{
public:
    Libhid();

    static QString GetHardwareId();
};

#endif // LIBHID_H
