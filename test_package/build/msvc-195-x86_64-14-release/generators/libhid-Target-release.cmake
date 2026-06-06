# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(libhid_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(libhid_FRAMEWORKS_FOUND_RELEASE "${libhid_FRAMEWORKS_RELEASE}" "${libhid_FRAMEWORK_DIRS_RELEASE}")

set(libhid_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET libhid_DEPS_TARGET)
    add_library(libhid_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET libhid_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${libhid_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${libhid_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### libhid_DEPS_TARGET to all of them
conan_package_library_targets("${libhid_LIBS_RELEASE}"    # libraries
                              "${libhid_LIB_DIRS_RELEASE}" # package_libdir
                              "${libhid_BIN_DIRS_RELEASE}" # package_bindir
                              "${libhid_LIBRARY_TYPE_RELEASE}"
                              "${libhid_IS_HOST_WINDOWS_RELEASE}"
                              libhid_DEPS_TARGET
                              libhid_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "libhid"    # package_name
                              "${libhid_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libhid_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET libhid::libhid
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${libhid_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${libhid_LIBRARIES_TARGETS}>
                 )

    if("${libhid_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET libhid::libhid
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     libhid_DEPS_TARGET)
    endif()

    set_property(TARGET libhid::libhid
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${libhid_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET libhid::libhid
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${libhid_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET libhid::libhid
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${libhid_LIB_DIRS_RELEASE}>)
    set_property(TARGET libhid::libhid
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${libhid_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET libhid::libhid
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${libhid_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(libhid_LIBRARIES_RELEASE libhid::libhid)
