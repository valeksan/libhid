#include "libhid.h"

#if defined( Q_OS_WIN )
#include "imp/windows_manager.hpp"
#elif defined( Q_OS_LINUX )
#include "implementation/linux/linux_manager.hpp"
#elif defined( Q_OS_MAC )
#include "implementation/mac/mac_manager.hpp"
#endif
#include "imp/md5.hpp"
#include "imp/util.hpp"

#include <QDebug>
#include <cstring>

using namespace system_info;

Libhid::Libhid()
{
}

QString Libhid::GetHardwareId()
{
    QString hid = "";
    try {
        qDebug() << "Выполняется метод GetHardwareId";
        // Получаем HID
        QString hid = NativeOSManager::GetHardwareProperties();
        if (hid.isEmpty()) {
            qWarning() << "Не удалось получить ни один из всех идентификаторов железа!";
            return hid;
        }
        // Получаем MD5 хеш данных о железе - есть наш UUID
        MD5 md5(hid.toStdString());
        QString md5hash = QString::fromStdString(md5.HexDigest());
        QString uuid = Util::HashToUUID(md5hash);

        // Копируем результат в буффер
        QByteArray data;
        data.reserve(uuid.size() + 1);
        std::memset(data.data(), '\0', uuid.size() + 1);
        std::memcpy(data.data(), uuid.toStdString().c_str(), uuid.size() + 1);
        hid = QString::fromLatin1(data);
    } catch (...) {
        qWarning() << "Непредсказуемая ошибка в GetHardwareId";
    }
    return hid;
}
