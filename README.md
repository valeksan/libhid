# libhid

[![CI](https://github.com/valeksan/libhid/actions/workflows/ci.yml/badge.svg)](https://github.com/valeksan/libhid/actions/workflows/ci.yml)

`libhid` is a small C++ shared library for generating a stable hardware-based
identifier for the current machine.

Despite the name, `libhid` is not a USB HID library. In this project, HID means
hardware identifier.

The library collects several OS-specific hardware properties, combines them,
calculates an MD5 hash, and formats the result as a UUID-like string. This can be
useful when an application developer needs a repeatable local machine identifier
without implementing separate Windows, Linux, and macOS hardware probes.

## When It Is Useful

- Binding application settings or local licenses to a specific machine.
- Generating a stable client or workstation identifier.
- Distinguishing machines in internal tools, diagnostics, or telemetry.
- Avoiding direct hardware probing code in the main application.

`libhid` does not provide cryptographic identity proof and should not be used as
the only security layer for licensing or access control. Hardware values can
change after system reinstall, disk replacement, motherboard replacement,
virtual machine changes, or restricted OS permissions.

## Supported Platforms

- Windows: uses WMI and system APIs.
- Linux: reads DMI, disk, and network information from system paths.
- macOS: uses IOKit and CoreFoundation.

## Hardware Property Sources

`libhid` combines several platform-specific values before hashing them. The
exact source values are intentionally not exposed as public API.

Windows sources:

- `Win32_ComputerSystemProduct.UUID`
- `Win32_OperatingSystem.SerialNumber`
- System drive `Win32_LogicalDisk.VolumeSerialNumber`
- `Win32_ComputerSystemProduct.IdentifyingNumber`
- `Win32_BaseBoard.SerialNumber`
- Non-Microsoft network adapter MAC address

Linux sources:

- `/sys/class/dmi/id/product_uuid`
- `/sys/class/dmi/id/product_serial`
- `/sys/class/dmi/id/board_serial`
- First matching `sda` or `hda` UUID from `/dev/disk/by-uuid/`
- First matching `eth*` or `enp*` MAC address from `/sys/class/net/`

macOS sources:

- `IOPlatformSerialNumber`
- `IOPlatformUUID`
- `IOMACAddress`
- Storage device `Serial Number`

Known limitations:

- Virtual machines, containers, and CI runners may expose incomplete or synthetic
  hardware values.
- Linux DMI files may require elevated permissions or may be unavailable.
- Network interface naming can vary, especially on Linux.
- Storage identifiers can change after disk replacement, repartitioning, or OS
  reinstall.
- macOS storage service names may vary across hardware generations.

## Public API

```cpp
#include "libhid/libhid.h"

#include <iostream>
#include <string>

int main()
{
    const std::string hardwareId = libhid::GetHardwareId();
    const std::string appHardwareId = libhid::GetHardwareId("my-product");

    if (hardwareId.empty()) {
        std::cerr << "Hardware ID is not available\n";
        return 1;
    }

    std::cout << hardwareId << '\n';
    std::cout << appHardwareId << '\n';
    return 0;
}
```

The returned value is a UUID-like string, for example:

```text
01234567-89ab-cdef-0123-456789abcdef
```

`libhid::GetHardwareId()` may return an empty string if the operating system does not
expose the required hardware properties or if access to those properties is
restricted. Callers should treat an empty value as "hardware ID unavailable" and
decide on an application-specific fallback.

Use `libhid::GetHardwareId("your-product-or-namespace")` when different applications
should receive different stable IDs on the same machine. Passing an empty string
keeps the same behavior as `libhid::GetHardwareId()`.

The older `LibHid::GetHardwareId()` class API remains available as a
compatibility wrapper.

## Identifier Stability

The generated ID is intended to be stable for the same machine while the
underlying hardware and OS-reported values remain stable. It can change after:

- Reinstalling or significantly reconfiguring the operating system.
- Replacing disks, network adapters, motherboard, or other hardware.
- Running inside a different virtual machine configuration.
- Losing permissions to read platform-specific hardware properties.

The value is useful as a practical machine fingerprint, but it is not a permanent
or tamper-proof device identity.

## Security Notes

`libhid` uses MD5 only to normalize collected hardware properties into a compact
UUID-like string. It is not used as a cryptographic security primitive.

Do not rely on this ID as the only proof for licensing, authentication, or access
control. For security-sensitive systems, combine it with server-side validation,
signed license data, user accounts, or another trusted mechanism.

An application namespace is not a secret and should not be treated as a password
or cryptographic salt. It is only a practical way to separate IDs between
products or internal tools.

## Build With CMake

Current library version: `0.1.0`.

Requirements:

- CMake 3.16 or newer.
- A C++ compiler with C++11 support.

Configure and build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Or use CMake presets if your CMake version supports them. Presets use the
`Ninja` generator, so make sure `ninja` is available in `PATH`.

```sh
cmake --preset release
cmake --build --preset release
ctest --preset release
```

Run tests:

```sh
ctest --test-dir build --output-on-failure
```

Developer warnings are enabled by default. They can be disabled with:

```sh
cmake -S . -B build -DLIBHID_ENABLE_WARNINGS=OFF
```

Example applications are enabled by default. They can be disabled with:

```sh
cmake -S . -B build -DLIBHID_BUILD_EXAMPLES=OFF
```

Tests are enabled by default. They can be disabled with:

```sh
cmake -S . -B build -DLIBHID_BUILD_TESTS=OFF
```

Install rules are enabled by default. They can be disabled with:

```sh
cmake -S . -B build -DLIBHID_INSTALL=OFF
```

The built library is written to `../libs` relative to the project directory.

On Windows with MinGW, the output files are named:

```text
libhid.dll
libhid.dll.a
libhid_print_hardware_id.exe
```

Run the example:

```sh
../libs/libhid_print_hardware_id
```

When running a MinGW build manually on Windows, make sure the compiler runtime
DLLs are available in `PATH`. Qt Creator usually configures this environment
automatically.

## Use From Another CMake Project

After installing `libhid`, consume it with:

```cmake
find_package(libhid CONFIG REQUIRED)

target_link_libraries(your_app PRIVATE libhid::libhid)
```

When using the repository directly as a subproject:

```cmake
add_subdirectory(path/to/libhid)

target_link_libraries(your_app PRIVATE libhid::libhid)
```

## Build In Qt Creator

Open the project as a CMake project by selecting `CMakeLists.txt`.
Qt Creator will create its own local build directory, which is ignored by Git.

The recommended public include path is `libhid/libhid.h`. A compatibility
wrapper is also kept at `libhid.h` for existing code.

## Migrating From qmake

The project now uses CMake as its build system. The old `libhid.pro` qmake file
has been removed.

Recommended migration path:

- Open or import `CMakeLists.txt` instead of `libhid.pro`.
- Replace direct source-file inclusion with linking to `libhid` or
  `libhid::libhid`.
- Prefer `#include "libhid/libhid.h"` for new code.
- Keep `#include "libhid.h"` only for existing code that has not been migrated
  yet.
- Remove qmake-specific build assumptions from downstream projects over time.

## Continuous Integration

GitHub Actions builds and tests the CMake project on Windows, Linux, and macOS in Debug
and Release configurations. The workflow also verifies the install step so the
CMake package export remains usable by downstream projects.

## Versioning

`libhid` follows semantic versioning while moving toward a stable `1.0.0`
release.

Before `1.0.0`, minor versions may still include API adjustments, but the project
tries to keep source compatibility whenever practical. After `1.0.0`, breaking
API or ABI changes should require a major version bump.

Breaking changes include removing public functions, changing public signatures,
changing installed CMake target names, changing public header paths, or removing
documented compatibility wrappers such as `LibHid`.

## Notes For Developers

- Debug builds define `LIB_DEBUG` and may print diagnostic messages to standard
  output or standard error.
- The library exports its public API from a shared library using the generated
  `LIBHID_EXPORT` macro. The old `SHAREDTESTLIB_EXPORT` name is kept as a
  compatibility alias.
- The hardware ID depends on values reported by the operating system and may be
  empty if required properties are unavailable.

## License

This project is licensed under the MIT License. See `LICENSE` for details.
