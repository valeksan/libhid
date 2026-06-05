#include "mac_manager.hpp"

// MAC-specific include
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

// STD include
#include <cstdint>
#include <sstream>
#include <iostream>
#include <vector>

namespace system_info {

std::string CFStringToStdString(CFStringRef cfStr);

struct CFStringWrapper {
    explicit CFStringWrapper(CFStringRef cfStr)
    {
        this->cfStr = cfStr;
    }

    explicit CFStringWrapper(const std::string &str)
    {
        cfStr = CFStringCreateWithCString(kCFAllocatorDefault, str.c_str(), kCFStringEncodingASCII);

        if (!cfStr)
            throw std::exception();
    }

    ~CFStringWrapper()
    {
        if (cfStr) {
            CFRelease(cfStr);
        }
    }

    CFStringWrapper(const CFStringWrapper&) = delete;
    CFStringWrapper& operator=(const CFStringWrapper&) = delete;

    std::string GetStdString() const
    {
        return CFStringToStdString(cfStr);
    }

    CFStringRef cfStr = nullptr;
};

std::string CFStringToStdString(CFStringRef cfStr)
{
    if (!cfStr) {
        return "";
    }

    const CFIndex maxSize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cfStr), kCFStringEncodingUTF8) + 1;
    std::vector<char> buffer(static_cast<std::size_t>(maxSize));
    if (CFStringGetCString(cfStr, buffer.data(), maxSize, kCFStringEncodingUTF8)) {
        return std::string(buffer.data());
    }
    return "";
}

std::string NativeOSManager::GetHardwareProperties()
{
    std::string result = "";

    result += GetIOPlatformProperty( "IOPlatformSerialNumber" );
    result += GetIOPlatformProperty( "IOPlatformUUID" );
    result += GetIONetworkProperty( "IOMACAddress" );
    result += GetIOStorageProperty( "Serial Number" );

    return result;
}

std::string NativeOSManager::GetIOPlatformProperty(const std::string &property)
{
    std::string result = "";
    CFStringWrapper propertyCF(property);

    io_service_t service = IOServiceGetMatchingService(0, IOServiceMatching("IOPlatformExpertDevice"));

    if (service) {
        CFTypeRef registryPropertyVoidCF = IORegistryEntryCreateCFProperty(service, propertyCF.cfStr, 0, 0);
        if (registryPropertyVoidCF) {
            if (CFGetTypeID(registryPropertyVoidCF) == CFStringGetTypeID()) {
                CFStringWrapper registryPropertyCF((CFStringRef)registryPropertyVoidCF);
                result = registryPropertyCF.GetStdString();
            } else {
                CFRelease(registryPropertyVoidCF);
            }
        }
        IOObjectRelease(service);
    }
#ifdef LIB_DEBUG
    std::cout << "Property \'" << property << "\' of service \'IOPlatformExpertDevice\'"
              << " = \'" << result << "\' " << std::endl;
#endif
    return result;
}

std::string NativeOSManager::GetIONetworkProperty(const std::string &property)
{
    std::string result = "";
    CFStringWrapper propertyCF(property);

    io_service_t service = IOServiceGetMatchingService(
                kIOMasterPortDefault, IOServiceMatching("IOEthernetInterface"));

    if (service) {
        CFTypeRef registryPropertyVoidCF = IORegistryEntryCreateCFProperty(service, propertyCF.cfStr, 0, 0);

        const size_t MACAddressLength = 6;
        CFDataRef registryData = (CFDataRef)registryPropertyVoidCF;
        uint8_t MACAddrBuffer[MACAddressLength];
        if (registryPropertyVoidCF
                && CFGetTypeID(registryPropertyVoidCF) == CFDataGetTypeID()
                && CFDataGetLength(registryData) >= static_cast<CFIndex>(MACAddressLength))
        {
            CFRange range = CFRangeMake(0, static_cast<CFIndex>(MACAddressLength));
            CFDataGetBytes(registryData, range, MACAddrBuffer);

            std::ostringstream ss;
            for (size_t i = 0; i < MACAddressLength; i++) {
                if (i != 0)
                    ss << ":";
                ss.width(2);
                ss.fill('0');
                ss << std::hex << (int)(MACAddrBuffer[i]);
            }

            result = ss.str();
        }

        if (registryPropertyVoidCF) {
            CFRelease(registryPropertyVoidCF);
        }
        IOObjectRelease(service);
    }
#ifdef LIB_DEBUG
    std::cout << "Property \'" << property << "\' of service \'IOEthernetInterface\'"
              << " = \'" << result << "\' " << std::endl;
#endif
    return result;
}

std::string NativeOSManager::GetIOStorageProperty(const std::string &property)
{
    std::string result = "";
    CFStringWrapper propertyCF(property);

    io_service_t service = IOServiceGetMatchingService(
                kIOMasterPortDefault, IOServiceMatching("IOAHCIBlockStorageDevice"));

    if (service) {
        CFMutableDictionaryRef properties = nullptr;
        if (IORegistryEntryCreateCFProperties(service, &properties, 0, 0) == KERN_SUCCESS && properties) {
            CFDictionaryRef deviceProperties = (CFDictionaryRef)(CFDictionaryGetValue(properties, CFSTR("Device Characteristics")));
            if (deviceProperties && CFGetTypeID(deviceProperties) == CFDictionaryGetTypeID()) {
                CFTypeRef serialNumber = CFDictionaryGetValue(deviceProperties, propertyCF.cfStr);
                if (serialNumber && CFGetTypeID(serialNumber) == CFStringGetTypeID()) {
                    result = CFStringToStdString((CFStringRef)serialNumber);
                }
            }
            CFRelease(properties);
        }
        IOObjectRelease(service);
    }
#ifdef LIB_DEBUG
    std::cout << "Property \'Serial Number\' of service \'IOAHCIBlockStorageDevice\'"
              << " = \'" << result << "\' " << std::endl;
#endif
    return result;
}

} // end namespace system_info
