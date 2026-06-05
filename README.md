# libhid

`libhid` is a small C++ shared library for generating a stable hardware-based
identifier for the current machine.

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

## Public API

```cpp
#include "libhid/libhid.h"

#include <iostream>
#include <string>

int main()
{
    const std::string hardwareId = LibHid::GetHardwareId();

    if (hardwareId.empty()) {
        std::cerr << "Hardware ID is not available\n";
        return 1;
    }

    std::cout << hardwareId << '\n';
    return 0;
}
```

The returned value is a UUID-like string, for example:

```text
01234567-89ab-cdef-0123-456789abcdef
```

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

Developer warnings are enabled by default. They can be disabled with:

```sh
cmake -S . -B build -DLIBHID_ENABLE_WARNINGS=OFF
```

Example applications are enabled by default. They can be disabled with:

```sh
cmake -S . -B build -DLIBHID_BUILD_EXAMPLES=OFF
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

## Build In Qt Creator

Open the project as a CMake project by selecting `CMakeLists.txt`.
Qt Creator will create its own local build directory, which is ignored by Git.

The old `libhid.pro` file is kept temporarily for compatibility during migration
from qmake to CMake.

The recommended public include path is `libhid/libhid.h`. A compatibility
wrapper is also kept at `libhid.h` for existing code.

## Notes For Developers

- Debug builds define `LIB_DEBUG` and may print diagnostic messages to standard
  output or standard error.
- The library exports `LibHid` from a shared library using the `LIBHID_EXPORT`
  macro. The old `SHAREDTESTLIB_EXPORT` name is kept as a compatibility alias.
- The hardware ID depends on values reported by the operating system and may be
  empty if required properties are unavailable.
