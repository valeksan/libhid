from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, collect_libs


class LibhidConan(ConanFile):
    name = "libhid"
    version = "0.1.0"
    license = "MIT"
    url = "https://github.com/valeksan/libhid"
    description = "Small C++ library for generating a stable hardware-based machine identifier"
    topics = ("hardware-id", "machine-id", "fingerprint", "cmake")
    package_type = "library"
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}
    exports_sources = (
        "CMakeLists.txt",
        "CMakePresets.json",
        "LICENSE",
        "README.md",
        "libhid.h",
        "cmake/*",
        "include/*",
        "src/*",
    )

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        toolchain = CMakeToolchain(self)
        toolchain.variables["LIBHID_BUILD_EXAMPLES"] = False
        toolchain.variables["LIBHID_BUILD_TESTS"] = False
        toolchain.variables["LIBHID_INSTALL"] = True
        toolchain.variables["BUILD_SHARED_LIBS"] = self.options.shared
        toolchain.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "LICENSE", src=self.source_folder, dst=self.package_folder)

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "libhid")
        self.cpp_info.set_property("cmake_target_name", "libhid::libhid")
        self.cpp_info.libs = collect_libs(self)

        if self.settings.os == "Windows":
            self.cpp_info.system_libs.extend(["ole32", "oleaut32", "shell32"])
        elif self.settings.os == "Macos":
            self.cpp_info.frameworks.extend(["CoreFoundation", "IOKit"])
