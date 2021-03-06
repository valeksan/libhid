#include "mac_manager.hpp"
#include "../log.hpp"
#include "../util.hpp"
#include "../version.hpp"

// MAC-specific include
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

// STD include
#include <sstream>
#include <iostream>

namespace system_info {

struct CFStringWrapper {
    CFStringWrapper(CFStringRef cfStr)
    {
        this->cfStr = cfStr;
    }

    CFStringWrapper(const std::string str)
    {
        cfStr = CFStringCreateWithCString(kCFAllocatorDefault, str.c_str(), kCFStringEncodingASCII);

        if (!cfStr)
            throw std::exception();
    }

    ~CFStringWrapper()
    {
        CFRelease(cfStr);
    }

    std::string GetStdString()
    {
        if (cfStr ) {
            char buffer[ 500 ];
            if (CFStringGetCString(cfStr, buffer, 500, kCFStringEncodingASCII))
                return std::string(buffer);
        }
        return "";
    }

    CFStringRef cfStr = nullptr;
};

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
        CFStringWrapper registryPropertyCF((CFStringRef)registryPropertyVoidCF);

        result = registryPropertyCF.GetStdString();
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
        CFDataGetBytes(registryData, {.location = 0, .length = MACAddressLength}, MACAddrBuffer);

        std::ostringstream ss;
        for (size_t i = 0; i < MACAddressLength; i++) {
            if (i != 0)
                ss << ":";
            ss.width(2);
            ss.fill('0');
            ss << std::hex << (int)(MACAddrBuffer[i]);
        }

        result = ss.str();

        CFRelease(registryPropertyVoidCF);
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
        IORegistryEntryCreateCFProperties(service, &properties, 0, 0);
        CFRetain(properties); /* lock properties (Without blocking the collection,
            there may be an error of unsynchronized access to the collection) */

        CFDictionaryRef deviceProperties = (CFDictionaryRef)(CFDictionaryGetValue(properties, CFSTR("Device Characteristics")));
        CFRetain( deviceProperties ); // lock deviceProperties

        CFStringWrapper serialNumber((CFStringRef)(CFDictionaryGetValue(deviceProperties, propertyCF.cfStr)));
        result = serialNumber.GetStdString();

        CFRelease(deviceProperties);
        CFRelease(properties);
        IOObjectRelease(service);
    }
#ifdef LIB_DEBUG
    std::cout << "Property \'Serial Number\' of service \'IOAHCIBlockStorageDevice\'"
              << " = \'" << result << "\' " << std::endl;
#endif
    return result;
}

} // end namespace system_info
