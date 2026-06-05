cmake_minimum_required(VERSION 3.16)

project(libhid
    LANGUAGES CXX
)

include(GNUInstallDirs)

add_library(libhid SHARED)

target_sources(libhid
    PRIVATE
        imp/md5.cpp
        imp/md5.hpp
        imp/util.cpp
        imp/util.hpp
        libhid.cpp
        libhid.h
)

target_compile_features(libhid
    PUBLIC
        cxx_std_11
)

target_include_directories(libhid
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(libhid
    PRIVATE
        SHAREDTESTLIB_LIBRARY
        $<$<CONFIG:Debug>:LIB_DEBUG>
)

if(WIN32)
    target_sources(libhid
        PRIVATE
            imp/windows_manager.cpp
            imp/windows_manager.hpp
    )

    target_compile_definitions(libhid
        PRIVATE
            _WIN32_WINNT=0x0600
            _WIN32_LEAN_AND_MEAN
            OS_WINDOWS
    )

    target_link_libraries(libhid
        PRIVATE
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
            imp/mac_manager.cpp
            imp/mac_manager.hpp
    )

    target_compile_definitions(libhid PRIVATE OS_MAC)

    target_link_libraries(libhid
        PRIVATE
            "${COREFOUNDATION_LIBRARY}"
            "${IOKIT_LIBRARY}"
    )
elseif(UNIX)
    target_sources(libhid
        PRIVATE
            imp/linux_manager.cpp
            imp/linux_manager.hpp
    )

    target_compile_definitions(libhid PRIVATE OS_LINUX)
else()
    message(FATAL_ERROR "Unsupported platform for libhid")
endif()

set_target_properties(libhid
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../libs"
)

install(TARGETS libhid
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(FILES libhid.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
