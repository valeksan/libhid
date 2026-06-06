cmake_minimum_required(VERSION 3.16)

project(test_package LANGUAGES CXX)

find_package(libhid CONFIG REQUIRED)

add_executable(test_package src/test_package.cpp)
target_link_libraries(test_package PRIVATE libhid::libhid)

enable_testing()
add_test(NAME test_package COMMAND test_package)
