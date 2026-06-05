#pragma once

#include <string>

struct IWbemServices;
using BSTR = wchar_t*;
using GUID = struct _GUID;
using CLSID = GUID;
using IID = GUID;

namespace system_info {

/**
 * @brief Class for getting information about computer hardware
 * in OS Windows
 */
class NativeOSManager {
 public:
    /**
     * @brief Get a string with iron identifiers (wrapper)
     * @return String with hardware identifiers following one after another
     */
    static std::string GetHardwareProperties();

 private:
    /**
     * @brief Convert string to lowercase
     * @param string Result with lowercase characters
     */
    static void ToLower(std::wstring &string);

    /**
     * @brief Get a string with hardware identifiers
     * @return String with hardware identifiers following one after another
     */
    static std::wstring GetWmiProperties();

    /**
     * @brief Get the value of a class property via WMI
     * @param services Interface for interacting with WMI
     * @param classname Target class
     * @param property Target property of the class
     * @param check Checking for empty value
     * @return The value of the specified class property
     */
    static std::wstring GetWmiProperty(IWbemServices *services, const wchar_t *classname, const wchar_t *property, bool check = true);

    /**
     * @brief Get hard disk partition ID
     * @param services Interface for interacting with WMI
     * @param check Is the algorithm older than version 1.2?
     * @return Hard disk partition ID value
     */
    static std::wstring GetWmiPropertyForHdd(IWbemServices *services, bool check);

    /**
     * @brief Get MAC-address
     * @param services Interface for interacting with WMI
     * @return MAC-address value
     */
    static std::wstring GetWmiPropertyForNetworkAdapter(IWbemServices *services);

    /**
     * @brief Process system value
     * @param prop BSTR string
     * @param check Checking for an empty value.
     * If enabled, an empty system value will be treated as an empty string
     * @return std::wstring value
     */
    static std::wstring ProcessWmiProperty(BSTR prop, bool check);

 private:
    const static CLSID local_CLSID_WbemLocator;
    const static IID local_IID_IWbemLocator;
};

} // end namespace system_info
