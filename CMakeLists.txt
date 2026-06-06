cmake_minimum_required(VERSION 3.16)

project(libhid
    VERSION 0.1.0
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)

option(LIBHID_ENABLE_WARNINGS "Enable additional compiler warnings for libhid developers" ON)
option(LIBHID_BUILD_EXAMPLES "Build libhid example applications" ON)
option(LIBHID_BUILD_TESTS "Build libhid tests" ON)
option(LIBHID_INSTALL "Generate libhid install rules" ON)
option(BUILD_SHARED_LIBS "Build libhid as a shared library" ON)

add_library(libhid)
add_library(libhid::libhid ALIAS libhid)

target_sources(libhid
    PRIVATE
        src/imp/hardware_id_builder.cpp
        src/imp/hardware_id_builder.hpp
        src/imp/md5.cpp
        src/imp/md5.hpp
        src/imp/util.cpp
        src/imp/util.hpp
        src/libhid.cpp
        include/libhid/libhid.h
        include/libhid/libhid_c.h
        libhid.h
        libhid_c.h
)

target_compile_features(libhid
    PUBLIC
        cxx_std_11
)

target_include_directories(libhid
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

generate_export_header(libhid
    BASE_NAME LIBHID
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/include/libhid/libhid_export.h"
)

target_compile_definitions(libhid PRIVATE $<$<CONFIG:Debug>:LIB_DEBUG>)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(libhid PUBLIC LIBHID_STATIC_DEFINE)
endif()

if(LIBHID_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(libhid PRIVATE /W4)
    else()
        target_compile_options(libhid PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

set(libhid_platform_link_scope PRIVATE)
if(NOT BUILD_SHARED_LIBS)
    set(libhid_platform_link_scope PUBLIC)
endif()

if(WIN32)
    set_target_properties(libhid
        PROPERTIES
            PREFIX ""
            IMPORT_PREFIX ""
    )

    target_sources(libhid
        PRIVATE
            src/imp/windows_manager.cpp
            src/imp/windows_manager.hpp
    )

    target_compile_definitions(libhid
        PRIVATE
            _WIN32_WINNT=0x0600
            _WIN32_LEAN_AND_MEAN
            OS_WINDOWS
    )

    target_link_libraries(libhid
        ${libhid_platform_link_scope}
            ole32
            oleaut32
            shell32
    )

    if(MINGW)
        target_compile_definitions(libhid PRIVATE INITGUID)
    endif()
elseif(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)

    target_sources(libhid
        PRIVATE
            src/imp/mac_manager.cpp
            src/imp/mac_manager.hpp
    )

    target_compile_definitions(libhid PRIVATE OS_MAC)

    target_link_libraries(libhid
        ${libhid_platform_link_scope}
            "${COREFOUNDATION_LIBRARY}"
            "${IOKIT_LIBRARY}"
    )
elseif(UNIX)
    target_sources(libhid
        PRIVATE
            src/imp/linux_manager.cpp
            src/imp/linux_manager.hpp
    )

    target_compile_definitions(libhid PRIVATE OS_LINUX)
else()
    message(FATAL_ERROR "Unsupported platform for libhid")
endif()

set_target_properties(libhid
    PROPERTIES
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
)

if(LIBHID_INSTALL)
    set(libhid_cmake_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/libhid")

    install(TARGETS libhid
        EXPORT libhidTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

    install(FILES libhid.h libhid_c.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    install(FILES include/libhid/libhid.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libhid")
    install(FILES include/libhid/libhid_c.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libhid")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/libhid/libhid_export.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libhid")

    install(EXPORT libhidTargets
        FILE libhidTargets.cmake
        NAMESPACE libhid::
        DESTINATION "${libhid_cmake_install_dir}"
    )

    configure_package_config_file(
        cmake/libhidConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/libhidConfig.cmake"
        INSTALL_DESTINATION "${libhid_cmake_install_dir}"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/libhidConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libhidConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libhidConfigVersion.cmake"
        DESTINATION "${libhid_cmake_install_dir}"
    )
endif()

if(LIBHID_BUILD_EXAMPLES)
    add_executable(libhid_print_hardware_id examples/print_hardware_id.cpp)

    target_link_libraries(libhid_print_hardware_id PRIVATE libhid)

    set_target_properties(libhid_print_hardware_id
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
    )
endif()

if(LIBHID_BUILD_TESTS)
    enable_testing()

    function(libhid_configure_windows_test_runtime test_name runtime_target)
        if(WIN32)
            get_filename_component(libhid_compiler_runtime_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
            set_tests_properties(${test_name}
                PROPERTIES
                    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:${runtime_target}>;PATH=path_list_prepend:${libhid_compiler_runtime_dir}"
            )
        endif()
    endfunction()

    add_executable(libhid_pure_functions_test
        tests/pure_functions.cpp
        src/imp/hardware_id_builder.cpp
        src/imp/md5.cpp
        src/imp/util.cpp
    )

    target_include_directories(libhid_pure_functions_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    set_target_properties(libhid_pure_functions_test
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
    )

    add_test(NAME libhid_pure_functions_test COMMAND libhid_pure_functions_test)

    set_tests_properties(libhid_pure_functions_test
        PROPERTIES
            TIMEOUT 10
    )
    libhid_configure_windows_test_runtime(libhid_pure_functions_test libhid_pure_functions_test)

    add_executable(libhid_smoke_test tests/smoke.cpp)

    target_link_libraries(libhid_smoke_test PRIVATE libhid)

    set_target_properties(libhid_smoke_test
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
    )

    add_test(NAME libhid_smoke_test COMMAND libhid_smoke_test)

    set_tests_properties(libhid_smoke_test
        PROPERTIES
            TIMEOUT 30
    )
    libhid_configure_windows_test_runtime(libhid_smoke_test libhid)

    add_executable(libhid_c_api_smoke_test tests/c_api_smoke.cpp)

    target_link_libraries(libhid_c_api_smoke_test PRIVATE libhid)

    set_target_properties(libhid_c_api_smoke_test
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
    )

    add_test(NAME libhid_c_api_smoke_test COMMAND libhid_c_api_smoke_test)

    set_tests_properties(libhid_c_api_smoke_test
        PROPERTIES
            TIMEOUT 30
    )
    libhid_configure_windows_test_runtime(libhid_c_api_smoke_test libhid)
endif()
