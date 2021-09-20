#include "linux_manager.hpp"
#include "imp/util.hpp"

// Linux include specific
#include <dirent.h>

// Standart include
#include <iostream>
#include <fstream>
#include <map>

namespace system_info {

QString NativeOSManager::GetHardwareProperties()
{
    std::string result = "";

    result += GetDmiProperty("product", "uuid");
    result += GetDmiProperty("product", "serial");
    result += GetDmiProperty("board", "serial");
    result += GetHDDSerialNumber();
    result += GetMACAddress();

    return QString::fromStdString(result);
}

std::string NativeOSManager::GetDmiProperty(const std::string &service, const std::string &property)
{
    std::string result = "";
    std::ifstream file("/sys/class/dmi/id/" + service + "_" + property);

    if (file.is_open()) {
        std::getline(file, result);
        file.close();
    }

#ifdef LIB_DEBUG
    std::cout << "Значение свойства \'" << property << "\' для \'" << service << "\'" << " = \'" << result << "\'";
#endif
    return result;
}

std::string NativeOSManager::GetHDDSerialNumber()
{
    // Открываем директорию с идентификаторами жестких дисков
    const std::string uuidFolder = "/dev/disk/by-uuid/";
    std::vector<std::string> uuids;

    if (!GetItemsFromFolder(uuidFolder, uuids, DT_LNK)) {
#ifdef LIB_DEBUG
        std::cerr << "Не смогли получить список идентификаторов в папке /dev/disk/by-uuid/";
#endif
        return "";
    }

    // Формируем словарь "служебное имя жесткого диска - UUID"
    // Коллекция упорядочена по ключу
    std::map<std::string, std::string> devices;
    char buffer[200];
    for (const std::string &uuid : uuids) {
        std::string uuidPath = uuidFolder + uuid;
        realpath(uuidPath.c_str(), buffer);
        // Вставка происходит с сортировкой по ключу
        devices[buffer] = uuid;
    }

    // Находим самое раннее имя sda или hda и возвращаем его UUID
    for (const auto &item : devices) {
        if (item.first.find("sda") != std::string::npos ||
                item.first.find("hda") != std::string::npos)
        {
#ifdef LIB_DEBUG
            std::cout << "Серийный номер жесткого диска: " + item.second;
#endif
            return item.second;
        }
    }
#ifdef LIB_DEBUG
    std::cerr << "Не смогли получить серийный номер жесткого диска";
#endif
    return "";
}

std::string NativeOSManager::GetMACAddress()
{
    // Открываем директорию с именами сетевых интерфейсов
    const std::string netFolder = "/sys/class/net/";
    std::vector<std::string> netNames;

    if (!GetItemsFromFolder(netFolder, netNames, DT_LNK)) {
#ifdef LIB_DEBUG
        std::cerr << "Не смогли получить список идентификаторов в папке /sys/class/net/";
#endif
        return "";
    }

    // Формируем словарь "служебное имя сетевого устройства - сетевой адрес"
    // Коллекция упорядочена по ключу
    std::map<std::string, std::string> netDevices;
    std::string tempAddress = "";
    for (const std::string &netName : netNames) {
        std::string addressPath = netFolder + netName + "/address";
        std::ifstream addressFile(addressPath);
        tempAddress = "";
        if (addressFile.is_open()) {
            std::getline(addressFile, tempAddress);
            addressFile.close();
        }
        netDevices[netName] = tempAddress;
    }

    // Находим самое раннее имя eth или enp и возвращаем его сетевой адрес
    for (const auto &item : netDevices) {
        if (item.first.find("eth") != std::string::npos
                || item.first.find( "enp" ) != std::string::npos)
        {
#ifdef LIB_DEBUG
            std::cout << "MAC-адрес: " + item.second;
#endif
            return item.second;
        }
    }
#ifdef LIB_DEBUG
    std::cerr << "Не смогли получить MAC-адрес";
#endif
    return "";
}

bool NativeOSManager::GetItemsFromFolder(const std::string &folder, std::vector<std::string> &itemList , int type)
{
    DIR *dir = nullptr;
    if (!(dir = opendir(folder.c_str()))) {
        return false;
    }
    dirent *dirp = nullptr;
    while ((dirp = readdir(dir))) {
        if (dirp->d_type == type)
            itemList.push_back(std::string(dirp->d_name));
    }
    closedir(dir);
    return true;
}

} // end namespace system_info
