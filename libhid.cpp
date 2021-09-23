#include "libhid.h"

#if defined(OS_WINDOWS)
#include "imp/windows_manager.hpp"
#elif defined(OS_LINUX)
#include "imp/linux_manager.hpp"
#elif defined(OS_MAC)
#include "imp/mac_manager.hpp"
#endif
#include "imp/md5.hpp"
#include "imp/util.hpp"

#include <cstring>
#include <iostream>

using namespace system_info;

LibHid::LibHid()
{
}

std::string LibHid::GetHardwareId()
{
    std::string hardwareIdResult = "";
    try {
        // Getting HID
        std::string hid = NativeOSManager::GetHardwareProperties();
        if (hid.empty()) {
#ifdef LIB_DEBUG
            std::cerr << "Couldn't get any of all hardware IDs! " << std::endl;
#endif
            return hid;
        }
        // Getting MD5 hash of data about hardware - there is our UUID
        MD5 md5(hid);
        std::string md5hash = md5.HexDigest();
        std::string uuid = Util::HashToUUID(md5hash);
        hardwareIdResult = uuid;
    } catch (...) {
#ifdef LIB_DEBUG
        std::cerr << "Unpredictable error in GetHardwareId! " << std::endl;
#endif
    }
    return hardwareIdResult;
}
