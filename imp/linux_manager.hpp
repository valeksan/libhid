#pragma once

#include <string>
#include <vector>

#include <QString>

namespace system_info {

/**
 * @brief Класс для работы с железом
 * в ОС Linux
 */
class NativeOSManager
{
public:
   /**
    * @brief Получить строку с идентификаторами железа
    * @return Строка с идентификаторами железа, следующими друг
    * за другом
    */
   static QString GetHardwareProperties();
private:
   /**
    * @brief Получить свойство DMI
    * @param service Целевой сервис
    * @param property Целевое свойство
    * @return Значение свойства указанного сервиса
    */
   static std::string GetDmiProperty(const std::string &service, const std::string &property);

   /**
    * @brief Получить серийный номер жесткого диска
    * @return Серийный номер жесткого диска
    */
   static std::string GetHDDSerialNumber();

   /**
    * @brief Получить МАС-адрес
    * @return МАС-адрес сетевого устройства
    */
   static std::string GetMACAddress();

   /**
    * @brief Получить дочерние объекты указанного типа из директории
    * @param folder Директория
    * @param itemList Результат (список объектов)
    * @param type Тип искомых объектов
    * @return True - если все ок, иначе false
    */
   static bool GetItemsFromFolder(const std::string &folder, std::vector<std::string> &itemList, int type);
};

} // end namespace system_info
