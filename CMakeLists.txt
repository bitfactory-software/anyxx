cmake_minimum_required(VERSION 3.28)
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)

message("CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message("VCPKG_ROOT: " $ENV{VCPKG_ROOT}/)

project(virtual_void CXX)

include(GNUInstallDirs)

include_directories( "./include" )

find_package(Catch2 3 REQUIRED)
enable_testing()

add_subdirectory ("src")
add_subdirectory ("test")
add_subdirectory ("tutorials")

include(CTest)


