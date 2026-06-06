#include "mac_manager.hpp"

// MAC-specific include
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

// STD include
#include <cstdint>
#include <exception>
#include <sstream>
#include <iostream>
#include <vector>

namespace system_info {

namespace {

std::string CFStringToStdString(CFStringRef cfStr);

template <typename T>
struct CFHolder {
    explicit CFHolder(T value = nullptr)
        : value(value)
    {
    }

    ~CFHolder()
    {
        if (value) {
            CFRelease(value);
        }
    }

    T get() const
    {
        return value;
    }

    T release()
    {
        T current = value;
        value = nullptr;
        return current;
    }

    T value = nullptr;
};

struct IOObjectHolder {
    explicit IOObjectHolder(io_object_t object = IO_OBJECT_NULL)
        : object(object)
    {
    }

    ~IOObjectHolder()
    {
        if (object) {
            IOObjectRelease(object);
        }
    }

    io_object_t get() const
    {
        return object;
    }

    io_object_t release()
    {
        io_object_t current = object;
        object = IO_OBJECT_NULL;
        return current;
    }

    io_object_t object = IO_OBJECT_NULL;
};

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

io_service_t FindMatchingService(const std::vector<const char *> &serviceNames)
{
    for (const char *serviceName : serviceNames) {
        IOObjectHolder service(IOServiceGetMatchingService(0, IOServiceMatching(serviceName)));
        if (service.get()) {
            return service.release();
        }
    }

    return IO_OBJECT_NULL;
}

} // namespace

std::string NativeOSManager::GetHardwareProperties()
{
    return GetHardwareProbeResult().combinedProperties;
}

HardwareProbeResult NativeOSManager::GetHardwareProbeResult()
{
    HardwareProbeResult result;

    result.Add("mac.platform_serial_number", GetIOPlatformProperty("IOPlatformSerialNumber"));
    result.Add("mac.platform_uuid", GetIOPlatformProperty("IOPlatformUUID"));
    result.Add("mac.mac_address", GetIONetworkProperty("IOMACAddress"));
    result.Add("mac.storage_serial_number", GetIOStorageProperty("Serial Number"));

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

    IOObjectHolder service(FindMatchingService({"IOEthernetInterface", "IO80211Interface"}));

    if (service.get()) {
        CFHolder<CFTypeRef> registryPropertyVoidCF(IORegistryEntryCreateCFProperty(service.get(), propertyCF.cfStr, 0, 0));

        const size_t MACAddressLength = 6;
        CFDataRef registryData = reinterpret_cast<CFDataRef>(registryPropertyVoidCF.get());
        uint8_t MACAddrBuffer[MACAddressLength];
        if (registryPropertyVoidCF.get()
                && CFGetTypeID(registryPropertyVoidCF.get()) == CFDataGetTypeID()
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
    }
#ifdef LIB_DEBUG
    std::cout << "Property \'" << property << "\' of service \'IOEthernetInterface/IO80211Interface\'"
              << " = \'" << result << "\' " << std::endl;
#endif
    return result;
}

std::string NativeOSManager::GetIOStorageProperty(const std::string &property)
{
    std::string result = "";
    CFStringWrapper propertyCF(property);

    IOObjectHolder service(FindMatchingService({
        "IOAHCIBlockStorageDevice",
        "IONVMeBlockStorageDevice",
        "IOBlockStorageDevice"
    }));

    if (service.get()) {
        CFMutableDictionaryRef properties = nullptr;
        if (IORegistryEntryCreateCFProperties(service.get(), &properties, 0, 0) == KERN_SUCCESS && properties) {
            CFHolder<CFMutableDictionaryRef> propertiesHolder(properties);
            CFDictionaryRef deviceProperties = reinterpret_cast<CFDictionaryRef>(
                CFDictionaryGetValue(propertiesHolder.get(), CFSTR("Device Characteristics")));
            if (deviceProperties && CFGetTypeID(deviceProperties) == CFDictionaryGetTypeID()) {
                CFTypeRef serialNumber = CFDictionaryGetValue(deviceProperties, propertyCF.cfStr);
                if (serialNumber && CFGetTypeID(serialNumber) == CFStringGetTypeID()) {
                    result = CFStringToStdString((CFStringRef)serialNumber);
                }
            }
        }
    }
#ifdef LIB_DEBUG
    std::cout << "Property \'Serial Number\' of service \'IOAHCIBlockStorageDevice/IONVMeBlockStorageDevice/IOBlockStorageDevice\'"
              << " = \'" << result << "\' " << std::endl;
#endif
    return result;
}

} // end namespace system_info
