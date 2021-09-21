#pragma once

#include <string>

namespace system_info {

/**
 * @brief Class for getting information about computer hardware
 * in MAC OS X
 */
class NativeOSManager {
 public:
    /**
     * @brief Get a string with hardware identifiers
     * @return String with hardware identifiers following one after another
     */
    static std::string GetHardwareProperties();

 private:
    /**
     * @brief Get the specified property of the IOPlatform service
     * @param property The property whose value you want to get
     * @return The value of the specified property
     */
    static std::string GetIOPlatformProperty(const std::string &property);

    /**
     * @brief Get the specified property of the IONetwork service
     * @param property The property whose value you want to get
     * @return The value of the specified property
     */
    static std::string GetIONetworkProperty(const std::string &property);

    /**
     * @brief Get the specified property of the IOStorage service
     * @param property The property whose value you want to get
     * @return The value of the specified property
     */
    static std::string GetIOStorageProperty(const std::string &property);
};

} // end namespace system_info
