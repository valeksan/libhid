#include "windows_manager.hpp"
#include "util.hpp"

// Windows-specific include
#include <shlobj.h>
#include <wbemcli.h>

// Standart include
#include <algorithm>
#include <cwctype>
#include <iostream>

#ifdef __MINGW32__
#include <KnownFolders.h>
#endif

namespace system_info {

namespace {

struct ComInitGuard
{
    ComInitGuard()
        : result(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))
    {
    }

    ~ComInitGuard()
    {
        if (SUCCEEDED(result)) {
            CoUninitialize();
        }
    }

    bool IsReady() const
    {
        return SUCCEEDED(result) || result == RPC_E_CHANGED_MODE;
    }

    HRESULT result;
};

bool EnsureComSecurity()
{
    const HRESULT result = CoInitializeSecurity(
        nullptr,
        -1,
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE,
        nullptr);

    return SUCCEEDED(result) || result == RPC_E_TOO_LATE;
}

template <typename T>
struct ComHolder
{
    ~ComHolder()
    {
        if (ptr) {
            ptr->Release();
        }
    }

    T *Get() const
    {
        return ptr;
    }

    T **Put()
    {
        return &ptr;
    }

    T *ptr = nullptr;
};

struct BSTRHolder
{
    explicit BSTRHolder(const wchar_t *str)
    {
        bstr = SysAllocString(str);
    }

    ~BSTRHolder()
    {
        if (bstr) {
            SysFreeString(bstr);
        }
    }

    BSTR bstr = nullptr;
};

struct VariantHolder
{
    VariantHolder()
    {
        VariantInit(&value);
    }

    ~VariantHolder()
    {
        VariantClear(&value);
    }

    VARIANT value;
};

} // namespace

// In mingw CLSID_WbemLocator and IID_IWbemLocator give undefined reference, we use our "analog"
const CLSID NativeOSManager::local_CLSID_WbemLocator = {0x4590F811, 0x1D3A, 0x11D0, {0x89, 0x1F, 0, 0xAA, 0, 0x4B, 0x2E, 0x24}};
const IID NativeOSManager::local_IID_IWbemLocator = {0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0, 0xAA, 0, 0x4B, 0x2E, 0x24}};

std::string NativeOSManager::GetHardwareProperties()
{
    ComInitGuard comInit;
    if (!comInit.IsReady()) {
#ifdef LIB_DEBUG
        std::cerr << "COM initialization failed for Windows hardware detection." << std::endl;
#endif
        return std::string();
    }

    if (!EnsureComSecurity()) {
#ifdef LIB_DEBUG
        std::cerr << "COM security initialization failed for Windows hardware detection." << std::endl;
#endif
        return std::string();
    }

    return GetHardwareProbeResult().combinedProperties;
}

std::wstring NativeOSManager::GetWmiProperties()
{
    std::wstring result;

    ComHolder<IWbemLocator> locator;
    const HRESULT locatorResult = CoCreateInstance(
        local_CLSID_WbemLocator,
        nullptr,
        CLSCTX_INPROC_SERVER,
        local_IID_IWbemLocator,
        reinterpret_cast<void **>(locator.Put()));
    if (FAILED(locatorResult)) {
        return result;
    }

    ComHolder<IWbemServices> services;
    BSTRHolder net(L"ROOT\\CIMV2");
    const HRESULT connectResult = locator.Get()->ConnectServer(
        net.bstr,
        nullptr,
        nullptr,
        nullptr,
        WBEM_FLAG_CONNECT_USE_MAX_WAIT,
        nullptr,
        nullptr,
        services.Put());
    if (FAILED(connectResult)) {
        return result;
    }

    const HRESULT blanketResult = CoSetProxyBlanket(
        services.Get(),
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        nullptr,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE);
    if (FAILED(blanketResult)) {
        return result;
    }

    try {
        result += GetWmiProperty(services.Get(), L"Win32_ComputerSystemProduct", L"UUID");
        result += GetWmiProperty(services.Get(), L"Win32_OperatingSystem", L"SerialNumber");
        result += GetWmiPropertyForHdd(services.Get(), true);
        result += GetWmiProperty(services.Get(), L"Win32_ComputerSystemProduct", L"IdentifyingNumber");
        result += GetWmiProperty(services.Get(), L"Win32_BaseBoard", L"SerialNumber");
        result += GetWmiPropertyForNetworkAdapter(services.Get());
    } catch (...) {
        result.clear();
    }

    return result;
}

HardwareProbeResult NativeOSManager::GetHardwareProbeResult()
{
    HardwareProbeResult result;

    ComInitGuard comInit;
    if (!comInit.IsReady()) {
#ifdef LIB_DEBUG
        std::cerr << "COM initialization failed for Windows hardware detection." << std::endl;
#endif
        return result;
    }

    if (!EnsureComSecurity()) {
#ifdef LIB_DEBUG
        std::cerr << "COM security initialization failed for Windows hardware detection." << std::endl;
#endif
        return result;
    }

    ComHolder<IWbemLocator> locator;
    const HRESULT locatorResult = CoCreateInstance(
        local_CLSID_WbemLocator,
        nullptr,
        CLSCTX_INPROC_SERVER,
        local_IID_IWbemLocator,
        reinterpret_cast<void **>(locator.Put()));
    if (FAILED(locatorResult)) {
        return result;
    }

    ComHolder<IWbemServices> services;
    BSTRHolder net(L"ROOT\\CIMV2");
    const HRESULT connectResult = locator.Get()->ConnectServer(
        net.bstr,
        nullptr,
        nullptr,
        nullptr,
        WBEM_FLAG_CONNECT_USE_MAX_WAIT,
        nullptr,
        nullptr,
        services.Put());
    if (FAILED(connectResult)) {
        return result;
    }

    const HRESULT blanketResult = CoSetProxyBlanket(
        services.Get(),
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        nullptr,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE);
    if (FAILED(blanketResult)) {
        return result;
    }

    try {
        result.Add("windows.system_product_uuid",
                   Util::WstringToString(GetWmiProperty(services.Get(), L"Win32_ComputerSystemProduct", L"UUID")));
        result.Add("windows.os_serial_number",
                   Util::WstringToString(GetWmiProperty(services.Get(), L"Win32_OperatingSystem", L"SerialNumber")));
        result.Add("windows.system_drive_volume_serial",
                   Util::WstringToString(GetWmiPropertyForHdd(services.Get(), true)));
        result.Add("windows.identifying_number",
                   Util::WstringToString(GetWmiProperty(services.Get(), L"Win32_ComputerSystemProduct", L"IdentifyingNumber")));
        result.Add("windows.baseboard_serial_number",
                   Util::WstringToString(GetWmiProperty(services.Get(), L"Win32_BaseBoard", L"SerialNumber")));
        result.Add("windows.network_adapter_mac",
                   Util::WstringToString(GetWmiPropertyForNetworkAdapter(services.Get())));
    } catch (...) {
        result = HardwareProbeResult();
    }

    return result;
}

std::wstring NativeOSManager::GetWmiProperty(IWbemServices *services, const wchar_t *classname, const wchar_t *property, bool check)
{
    std::wstring result;
    ComHolder<IEnumWbemClassObject> enumerator;

    try {
        std::wstring query = L"SELECT * FROM ";
        query += classname;
        BSTRHolder query_holder(query.c_str());
        BSTRHolder lang(L"WQL");

        const HRESULT queryResult = services->ExecQuery(
            lang.bstr,
            query_holder.bstr,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            nullptr,
            enumerator.Put());
        if (FAILED(queryResult)) {
            return result;
        }

        ComHolder<IWbemClassObject> object;
        ULONG returned = 0;
        const HRESULT nextResult = enumerator.Get()->Next(WBEM_INFINITE, 1, object.Put(), &returned);
        if (FAILED(nextResult) || returned == 0) {
            return result;
        }

        VariantHolder value;
        CIMTYPE type = 0;
        const HRESULT getResult = object.Get()->Get(property, 0, &value.value, &type, nullptr);
        if (FAILED(getResult)) {
            return result;
        }

        if (type == CIM_STRING && value.value.vt == VT_BSTR) {
            result += ProcessWmiProperty(V_BSTR(&value.value), check);
        }
    } catch (...) {
        result.clear();
    }

    return result;
}

std::wstring NativeOSManager::GetWmiPropertyForHdd(IWbemServices *services, bool check)
{
   std::wstring from = L"win32_logicaldisk WHERE deviceid=\"";
   PWSTR location;
   if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_System, KF_FLAG_DONT_UNEXPAND, nullptr, &location))) {
      from.append(1, *location);
      from.append(L":\"");
      CoTaskMemFree(location);
      return GetWmiProperty(services, from.c_str(), L"VolumeSerialNumber", check);
   }
#ifdef LIB_DEBUG
   std::cerr << "Hard disk partition ID could not be retrieved! " << std::endl;
#endif
   return std::wstring();
}

std::wstring NativeOSManager::ProcessWmiProperty(BSTR prop, bool check)
{
   std::wstring value(prop, SysStringLen(prop));
   ToLower(value);
   // Attention: all constants are lowercase
   const std::wstring empty_guid(L"00000000-0000-0000-0000-000000000000");
   const std::wstring broken_guid(L"ffffffff-ffff-ffff-ffff-ffffffffffff");
   const std::wstring empty_oem(L"to be filled by o.e.m.");
   if (check) {
      if ((value == empty_guid) || (value == broken_guid) || (value == empty_oem)) {
         return std::wstring();
      }
   }
   return value;
}

std::wstring NativeOSManager::GetWmiPropertyForNetworkAdapter(IWbemServices *services)
{
   return GetWmiProperty(services, L"Win32_NetworkAdapter WHERE Manufacturer != \'Microsoft\' AND NOT PNPDeviceID LIKE \'ROOT%\\\\\'", L"MACAddress", false);
}

void NativeOSManager::ToLower(std::wstring &string)
{
    std::transform(string.begin(), string.end(), string.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(std::towlower(ch)); });
}

} // end namespace system_info
