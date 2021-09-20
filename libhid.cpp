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

Libhid::Libhid()
{
}

std::string Libhid::GetHardwareId()
{
    std::string hardwareIdResult = "";
    try {
#ifdef LIB_DEBUG
        std::cout << "Выполняется метод GetHardwareId";
#endif
        // Получаем HID
        std::string hid = NativeOSManager::GetHardwareProperties();
        if (hid.empty()) {
#ifdef LIB_DEBUG
            std::cerr << "Не удалось получить ни один из всех идентификаторов железа!";
#endif
            return hid;
        }
        // Получаем MD5 хеш данных о железе - есть наш UUID
        MD5 md5(hid);
        std::string md5hash = md5.HexDigest();
        std::string uuid = Util::HashToUUID(md5hash);
        hardwareIdResult = uuid;
    } catch (...) {
#ifdef LIB_DEBUG
        std::cerr << "Непредсказуемая ошибка в GetHardwareId";
#endif
    }
    return hardwareIdResult;
}
