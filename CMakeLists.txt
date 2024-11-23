cmake_minimum_required(VERSION 3.28)
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

include(GNUInstallDirs)

message("CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message("VCPKG_ROOT: " $ENV{VCPKG_ROOT}/)

project(virtual_void CXX)

find_package(Catch2 3 REQUIRED)

include_directories( "./include" )

add_subdirectory ("src")
add_subdirectory ("test")
add_subdirectory ("tutorials")

include(CTest)
include(Catch)



