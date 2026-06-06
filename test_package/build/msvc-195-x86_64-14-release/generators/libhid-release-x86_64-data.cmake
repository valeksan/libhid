########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(libhid_COMPONENT_NAMES "")
if(DEFINED libhid_FIND_DEPENDENCY_NAMES)
  list(APPEND libhid_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES libhid_FIND_DEPENDENCY_NAMES)
else()
  set(libhid_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(libhid_PACKAGE_FOLDER_RELEASE "C:/Users/vi/.conan2/p/b/libhi3b11b0cb51193/p")
set(libhid_BUILD_MODULES_PATHS_RELEASE )


set(libhid_INCLUDE_DIRS_RELEASE "${libhid_PACKAGE_FOLDER_RELEASE}/include")
set(libhid_RES_DIRS_RELEASE )
set(libhid_DEFINITIONS_RELEASE )
set(libhid_SHARED_LINK_FLAGS_RELEASE )
set(libhid_EXE_LINK_FLAGS_RELEASE )
set(libhid_OBJECTS_RELEASE )
set(libhid_COMPILE_DEFINITIONS_RELEASE )
set(libhid_COMPILE_OPTIONS_C_RELEASE )
set(libhid_COMPILE_OPTIONS_CXX_RELEASE )
set(libhid_LIB_DIRS_RELEASE "${libhid_PACKAGE_FOLDER_RELEASE}/lib")
set(libhid_BIN_DIRS_RELEASE "${libhid_PACKAGE_FOLDER_RELEASE}/bin")
set(libhid_LIBRARY_TYPE_RELEASE SHARED)
set(libhid_IS_HOST_WINDOWS_RELEASE 1)
set(libhid_LIBS_RELEASE libhid)
set(libhid_SYSTEM_LIBS_RELEASE ole32 oleaut32 shell32)
set(libhid_FRAMEWORK_DIRS_RELEASE )
set(libhid_FRAMEWORKS_RELEASE )
set(libhid_BUILD_DIRS_RELEASE )
set(libhid_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(libhid_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libhid_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libhid_COMPILE_OPTIONS_C_RELEASE}>")
set(libhid_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libhid_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libhid_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libhid_EXE_LINK_FLAGS_RELEASE}>")


set(libhid_COMPONENTS_RELEASE )