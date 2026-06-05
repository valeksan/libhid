cmake_minimum_required(VERSION 3.16)

project(libhid
    LANGUAGES CXX
)

include(GNUInstallDirs)

add_library(libhid SHARED)

target_sources(libhid
    PRIVATE
        src/imp/md5.cpp
        src/imp/md5.hpp
        src/imp/util.cpp
        src/imp/util.hpp
        src/libhid.cpp
        include/libhid/libhid.h
        libhid.h
)

target_compile_features(libhid
    PUBLIC
        cxx_std_11
)

target_include_directories(libhid
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_definitions(libhid
    PRIVATE
        LIBHID_LIBRARY
        $<$<CONFIG:Debug>:LIB_DEBUG>
)

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
            src/imp/mac_manager.cpp
            src/imp/mac_manager.hpp
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
            src/imp/linux_manager.cpp
            src/imp/linux_manager.hpp
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
install(FILES include/libhid/libhid.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libhid")
