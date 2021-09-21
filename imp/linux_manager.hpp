#pragma once

#include <string>
#include <vector>

namespace system_info {

/**
 * @brief Class for getting information about computer hardware
 * in OS Linux
 */
class NativeOSManager {
 public:
    /**
     * @brief Get a string with hardware identifiers
     * @return A string with hardware IDs following each other
     */
    static std::string GetHardwareProperties();

 private:
    /**
     * @brief Get DMI Property
     * @param service Target service
     * @param property Target property
     * @return Property value of the specified service
     */
    static std::string GetDmiProperty(const std::string &service, const std::string &property);

    /**
     * @brief Get the serial number of the hard drive
     * @return Hard disk serial number
     */
    static std::string GetHDDSerialNumber();

    /**
     * @brief Get MAC-Address
     * @return MAC-Address of the network device
     */
    static std::string GetMACAddress();

    /**
     * @brief Get child objects of the specified type from the directory
     * @param folder Directory
     * @param itemList Result (list of objects)
     * @param type Type of objects to be searched
     * @return true if everything is ok, otherwise false
     */
    static bool GetItemsFromFolder(const std::string &folder, std::vector<std::string> &itemList, int type);
};

} // end namespace system_info
