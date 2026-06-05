#include "linux_manager.hpp"
#include "imp/util.hpp"

// Linux include specific
#include <dirent.h>

// Standart include
#include <iostream>
#include <fstream>
#include <map>

namespace system_info {

std::string NativeOSManager::GetHardwareProperties()
{
    std::string result = "";

    result += GetDmiProperty("product", "uuid");
    result += GetDmiProperty("product", "serial");
    result += GetDmiProperty("board", "serial");
    result += GetHDDSerialNumber();
    result += GetMACAddress();

    return result;
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
    std::cout << "Property value \'" << property << "\' for \'" << service << "\'" << " = \'" << result << "\' " << std::endl;
#endif
    return result;
}

std::string NativeOSManager::GetHDDSerialNumber()
{
    // Open the directory with hard disk identifiers
    const std::string uuidFolder = "/dev/disk/by-uuid/";
    std::vector<std::string> uuids;

    if (!GetItemsFromFolder(uuidFolder, uuids, DT_LNK)) {
#ifdef LIB_DEBUG
        std::cerr << "Couldn't get the list of IDs in '/dev/disk/by-uuid/' folder! " << std::endl;
#endif
        return "";
    }

    // Forming a dictionary "hard disk service name - UUID"
    // Collection sorted by key
    std::map<std::string, std::string> devices;
    char buffer[200];
    for (const std::string &uuid : uuids) {
        std::string uuidPath = uuidFolder + uuid;
        realpath(uuidPath.c_str(), buffer);
        // Insertion occurs with sorting by key
        devices[buffer] = uuid;
    }

    // Find the earliest sda or hda name and return its UUID
    for (const auto &item : devices) {
        if (item.first.find("sda") != std::string::npos ||
                item.first.find("hda") != std::string::npos)
        {
#ifdef LIB_DEBUG
            std::cout << "Hard disk serial number: " << item.second << " " << std::endl;
#endif
            return item.second;
        }
    }
#ifdef LIB_DEBUG
    std::cerr << "Couldn't get the serial number of the hard drive! " << std::endl;
#endif
    return "";
}

std::string NativeOSManager::GetMACAddress()
{
    // Open the directory with the names of network interfaces
    const std::string netFolder = "/sys/class/net/";
    std::vector<std::string> netNames;

    if (!GetItemsFromFolder(netFolder, netNames, DT_LNK)) {
#ifdef LIB_DEBUG
        std::cerr << "Couldn't get list of IDs in '/sys/class/net/' folder! " << std::endl;
#endif
        return "";
    }

    // We form the dictionary "service name of the network device - network address"
    // Collection sorted by key
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

    // Find the earliest eth or enp name and return its network address
    for (const auto &item : netDevices) {
        if (item.first.find("eth") != std::string::npos
                || item.first.find("enp") != std::string::npos)
        {
#ifdef LIB_DEBUG
            std::cout << "MAC-Address: " << item.second << " " << std::endl;
#endif
            return item.second;
        }
    }
#ifdef LIB_DEBUG
    std::cerr << "Couldn't get MAC-Address! " << std::endl;
#endif
    return "";
}

bool NativeOSManager::GetItemsFromFolder(const std::string &folder, std::vector<std::string> &itemList, int type)
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
